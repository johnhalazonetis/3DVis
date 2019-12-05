import cv2
import numpy as np
import yaml


def find_contours(input_image):
    gray = cv2.cvtColor(input_image, cv2.COLOR_BGR2GRAY)
    ret, gray = cv2.threshold(gray, 127, 255, 0)
    contours, hier = cv2.findContours(gray, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    return contours


def find_blob_center(input_image, contours, area_lower, area_upper):
    for cnt in contours:
        if area_lower < cv2.contourArea(cnt) < area_upper:
            M = cv2.moments(cnt)

        cX = int(M["m10"] / M["m00"])
        cY = int(M["m01"] / M["m00"])

        cv2.circle(input_image, (cX, cY), int(2), (0, 0, 255), int(1))


def simplify_contours(output_image, contours, scaling_factor):
    for cnt in contours:
        epsilon = scaling_factor * cv2.arcLength(cnt, True)
        approx = cv2.approxPolyDP(cnt, epsilon, True)
        cv2.drawContours(output_image, [approx], 0, (0, 0, 255), 1)


def remove_noise(input_image):
    kernel = np.ones((3, 3), np.uint8)
    output = cv2.morphologyEx(input_image, cv2.MORPH_OPEN, kernel, iterations=2)
    return output


def yaml_loader(file_path):
    # Loads a yaml file
    with open(file_path, "r") as file_descriptor:
        file_data = yaml.load(file_descriptor, Loader=yaml.FullLoader)
    return file_data


# Define file name
filepath = "variables.yaml"
data = yaml_loader(filepath)
files = data.get('files')

# Define original input image
img = cv2.imread(str(files["binary_image"]))

# Further filtering
filtered = remove_noise(img)

contours = find_contours(filtered)

# find_blob_center(filtered, contours, 55, 5000)

# cv2.drawContours(filtered, contours, -1, (255, 0, 0), int(1))

# cv2.imshow('IMG', filtered)
# cv2.waitKey(0)

# TODO: Need to find formula to determine the value of scaling_factor
simplify_contours(filtered, contours, 0.0075)

cv2.imshow('IMG', filtered)
cv2.waitKey(0)
