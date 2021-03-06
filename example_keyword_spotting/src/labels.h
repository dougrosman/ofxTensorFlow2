/*
 * ofxTensorFlow2
 *
 * Copyright (c) 2021 ZKM | Hertz-Lab
 * Paul Bethge <bethge@zkm.de>
 * Dan Wilcox <dan.wilcox@zkm.de>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * This code has been developed at ZKM | Hertz-Lab as part of „The Intelligent 
 * Museum“ generously funded by the German Federal Cultural Foundation.
 */

#pragma once

#include <map>
#include <string>

typedef std::map<int, std::string> Labels;

static Labels labelsMap = {
    {0, "nothing"},
    {1, "nine"},
    {2, "yes"},
    {3, "no"},
    {4, "up"},
    {5, "down"},
    {6, "left"},
    {7, "right"},
    {8, "on"},
    {9, "off"},
    {10, "stop"},
    {11, "go"},
    {12, "zero"},
    {13, "one"},
    {14, "two"},
    {15, "three"},
    {16, "four"},
    {17, "five"},
    {18, "six"},
    {19, "seven"},
    {20, "eight"},
    {21, "backward"},
    {22, "bed"},
    {23, "bird"},
    {24, "cat"},
    {25, "dog"},
    {26, "follow"},
    {27, "forward"},
    {28, "happy"},
    {29, "house"},
    {30, "learn"},
    {31, "marvin"},
    {32, "sheila"},
    {33, "tree"},
    {34, "visual"},
    {35, "wow"}
};
