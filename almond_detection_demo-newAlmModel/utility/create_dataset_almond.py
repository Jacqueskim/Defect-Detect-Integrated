import os
from torchvision import transforms
import shutil
from PIL import Image, ImageDraw as D
import random
import numpy as np

# folder with good and bad almond pictures with no background
good_path = "almonds"
bad_path = "pistachio"
background_train = os.path.join("background_all")  #'..', 'BG-20k', 'train'
background_test = os.path.join("background_all")  #'..', 'BG-20k', 'testval'

# number of good and bad objects per image
max_num_good_almonds = 3  # max number of good almonds in an image
max_num_bad_almonds = 3  # max number of bad almonds in an image

# size of dataset including training and test
total_size = 30000

# size of an image
x_size = 168
y_size = 224

# margin from border to place almond images
margin_x = 10
margin_y = 10

# size of good/bad almond objects & bounding box - it will be randomly scaled up/down
box_size_x_init = 65
box_size_y_init = 65

# max/min random scale value of the box size
max_box_scale = 1.2  # maximum random scale of the box size
min_box_scale = .8  # minimum random scale of the box size

step = 1  # random pixel step for almond box location


def get_files(folder_path):
    file_list = []
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            file_list.append(os.path.join(root, file))
    file_list.sort()
    return file_list


def paste_image(image_bg, image_element, x, y, w, h, rotate=0, h_flip=False):
    image_bg_copy = image_bg.copy()
    image_element_copy = image_element.copy()

    if h_flip:
        image_element_copy = image_element_copy.transpose(Image.FLIP_LEFT_RIGHT)
    image_element_copy = image_element_copy.rotate(rotate, expand=True)
    image_element_copy = image_element_copy.resize(size=(w, h))
    _, _, _, alpha = image_element_copy.split()
    # image_element_copy's width and height will change after rotation
    w = image_element_copy.width
    h = image_element_copy.height
    x0 = x
    y0 = y
    x1 = x0 + w
    y1 = y0 + h

    image_bg_copy.paste(image_element_copy, box=(x0, y0, x1, y1), mask=alpha)
    return image_bg_copy


def add_image(image_background, image_obj):

    # scale the image of object
    scale = random.uniform(min_box_scale, max_box_scale)
    box_size_x = int(box_size_x_init * scale)
    box_size_y = int(box_size_y_init * scale)

    # location
    loc_x = random.randrange(margin_x, x_size - box_size_x - margin_x, step)
    loc_y = random.randrange(margin_y, y_size - box_size_y - margin_y, step)
    image_background = paste_image(image_background, image_obj, loc_x, loc_y, box_size_x, box_size_y,
                                   random.randrange(0, 360, 5))
    # bounding box
    bbox_x0 = loc_x
    bbox_y0 = loc_y
    bbox_x1 = loc_x + box_size_x
    bbox_y1 = loc_y + box_size_y

    # draw = D.Draw(image_background)
    # draw.rectangle([(bbox_x0, bbox_y0), (bbox_x1, bbox_y1)], outline="blue")
    # image_background.show()
    return image_background, bbox_x0, bbox_y0, bbox_x1, bbox_y1


dataset_name = 'ALMOND'
out_folder_test = os.path.join(dataset_name, 'test')
out_folder_train = os.path.join(dataset_name, 'train')
out_folder_processed = os.path.join(dataset_name, 'processed')

if os.path.exists(dataset_name):
    print('Output folders exist. Delete!')
    shutil.rmtree(dataset_name)
try:
    os.makedirs(out_folder_test)
    os.makedirs(out_folder_train)
    os.makedirs(out_folder_processed)
except:
    print("Cannot make folders!")
    exit(0)

# transform
# hue, satutration and grayscale will change the color.
image_transform = transforms.Compose([
    transforms.ColorJitter(brightness=(0.85, 1.15), saturation=(0.75, 1.25), contrast=(0.75, 1.25), hue=(-0.4, 0.4)),
    transforms.RandomGrayscale(0.15),
    transforms.GaussianBlur(kernel_size=(5, 5), sigma=(0.1, 5))])

# read all good and bad files to lists
good_obj = get_files(good_path)
bad_obj = get_files(bad_path)

# read all background files to a list
background_train_obj = get_files(background_train)
background_test_obj = get_files(background_test)

cnt = 0

# create csv files
header = "img_name,label,width,height,x0,y0,x1,y1,num_of_boxes,img_width,img_height,bb_x0,bb_y0,bb_x1,bb_y1"
fptest = open(os.path.join(out_folder_processed, 'test_info.csv'), 'w')
fptest.write(header)

fptrain = open(os.path.join(out_folder_processed, 'train_info.csv'), 'w')
fptrain.write(header)

# set format to print arrays with commas
np.set_string_function(lambda x: repr(x).replace('(', '').replace(')', '')
                       .replace('array', '').replace("       ", ' '), repr=False)
for cnt in range(total_size):
    # load and resize a random background image
    if random.randrange(10) > 0:  # 90%
        is_train = True
        image_back = Image.open(background_train_obj[random.randrange(len(background_train_obj))])
        image_back = image_back.resize((x_size, y_size))
        image_mix = image_back.copy()
    else:
        is_train = False
        image_back = Image.open(background_test_obj[random.randrange(len(background_test_obj))])
        image_back = image_back.resize((x_size, y_size))
        image_mix = image_back.copy()

    # first place good almonds
    num_good = random.randrange(max_num_good_almonds + 1)
    for i in range(num_good):
        # find a random image
        image = Image.open(good_obj[random.randrange(len(good_obj))])
        # add the random image to the background
        image_mix, _, _, _, _ = add_image(image_mix, image)

    # now place bad almonds
    num_bad = 1 + random.randrange(max_num_bad_almonds)
    print(f'sample {cnt + 1} with {num_good} good and {num_bad} bad almonds')
    #  we need to add bounding box only for bad almonds
    csv_info = {'label': np.empty(0),
                'width': np.empty(0), 'height': np.empty(0), 'x0': np.empty(0), 'y0': np.empty(0),
                'x1': np.empty(0), 'y1': np.empty(0), 'num_of_boxes': 0, 'img_width': 0,
                'img_height': 0, 'bb_x0': 0, 'bb_y0': 0, 'bb_x1': 0, 'bb_y1': 0}
    for i in range(num_bad):
        # find a random image
        image = Image.open(bad_obj[random.randrange(len(bad_obj))])
        # add the random image to the background
        image_mix, box_x0, box_y0, box_x1, box_y1 = add_image(image_mix, image)

        # prepare csv
        csv_info['width'] = np.append(csv_info['width'], box_x1 - box_x0)
        csv_info['height'] = np.append(csv_info['height'], box_y1 - box_y0)
        csv_info['x0'] = np.append(csv_info['x0'], box_x0)
        csv_info['y0'] = np.append(csv_info['y0'], box_y0)
        csv_info['x1'] = np.append(csv_info['x1'], box_x1)
        csv_info['y1'] = np.append(csv_info['y1'], box_y1)

        csv_info['num_of_boxes'] = csv_info['num_of_boxes'] + 1
        csv_info['img_width'] = x_size
        csv_info['img_height'] = y_size
        csv_info['bb_x0'] = min(csv_info['x0'])
        csv_info['bb_y0'] = min(csv_info['y0'])
        csv_info['bb_x1'] = max(csv_info['x1'])
        csv_info['bb_y1'] = max(csv_info['y1'])

        csv_info['label'] = np.append(csv_info['label'], 1.0)

    fname = 'almond_' + str(cnt) + '__' + str(box_x0) + '_' + str(box_y0) + '_' + str(box_x1) + '_' + str(box_y1) + '.jpg'

    csv_line = '\n' + fname

    for i, item in enumerate(csv_info):
        if item in {'label', 'width', 'height', 'x0', 'y0', 'x1', 'y1'}:
            csv_line += ',"' + str(csv_info[item]) + '"'
        else:
            csv_line += ',' + str(csv_info[item])

    cnt += 1

    # apply transform to image
    image_mix = image_transform(image_mix)

    if is_train:
        image_mix.save(os.path.join(out_folder_train, fname))
        fptrain.write(csv_line)
    else:
        image_mix.save(os.path.join(out_folder_test, fname))
        fptest.write(csv_line)

fptrain.close()
fptest.close()

print(f'Creating {dataset_name}.zip')
shutil.make_archive(dataset_name, 'zip', base_dir=dataset_name)
