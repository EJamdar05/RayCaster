//
//  RandomColor.hpp
//  RayTrace
//
//  Created by Eshaq Jamdar on 11/6/23.
//

#ifndef RandomColor_hpp
#define RandomColor_hpp

#include <stdio.h>
class RandomColor {
public:
    ofColor getRandomColor() {
        return ofColor(rand() % 255, rand() % 255, rand() % 255); // Generate a random color
    }
};

#endif /* RandomColor_hpp */
