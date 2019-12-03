import numpy as np
import cv2
import yaml


def color_to_greyscale(input):
    gray = cv2.cvtColor(input, cv2.COLOR_BGR2GRAY)
    return gray


def canny_edge_detector(input, minVal, maxVal):
    edges = cv2.Canny(input, minVal, maxVal)
    return edges


def color_detector(input, lower_range, upper_range):
    mask = cv2.inRange(input, lower_range, upper_range)
    return mask


def yaml_loader(filepath):
    # Loads a yaml file
    with open(filepath, "r") as file_descriptor:
        data = yaml.load(file_descriptor)
    return data


# Define file name
variables_filepath = "variables.yaml"
variables_data = yaml_loader(variables_filepath)
files = variables_data.get('files')

# Define original input image
original = cv2.imread(str(files["original_image"]))
A = cv2.cvtColor(original, cv2.COLOR_BGR2HSV)

B = color_to_greyscale(A)
# cv2.imshow('Gray Image', B)
# cv2.waitKey(0)

destination_filepath = str(files["save_images"] + "/bw.jpg")
cv2.imwrite(destination_filepath, B)

C = canny_edge_detector(B, 150, 200)
# cv2.imshow('Canny Edges', C)
# cv2.waitKey(0)

destination_filepath = str(files["save_images"] + "/edges.jpg")
cv2.imwrite(destination_filepath, C)

lower_range = np.array([100, 160, 50])
upper_range = np.array([130, 255, 255])
D = color_detector(A, lower_range, upper_range)
# cv2.imshow('Detect Blue', D)
# cv2.waitKey(0)
# cv2.imwrite("../test_images/blue_binary.jpg", D)

lower_range = np.array([10, 85, 130])
upper_range = np.array([50, 255, 255])
E = color_detector(A, lower_range, upper_range)
cv2.imshow('Detect Yellow', E)
cv2.waitKey(0)
cv2.imwrite("../test_images/yellow_binary.jpg", E)

G = canny_edge_detector(E, 50, 150)
cv2.imshow('Canny Edges', G)
cv2.waitKey(0)

ret, thresh = cv2.threshold(B, 127, 255, 0, cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)
contours, hierarchy = cv2.findContours(thresh, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)

cnts = cv2.drawContours(E, contours[0], -1, (0, 255, 0), 1)

cv2.namedWindow("Result", cv2.WINDOW_NORMAL)
cv2.imshow("Result", cnts)
cv2.waitKey(0)
