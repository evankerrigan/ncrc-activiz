//
//  ProgressBar.h
//  
//
//  Created by mac on 6/19/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//


#include "Namespace.h"
#include "Color.h"
#include "Interval.h"
#include "Pattern.h"

LED_CONTROLLER_NAMESPACE_ENTER

class ProgressBar : public Pattern {
private:
    Color bgColor;  // background color
    Color barColor; // progress bar color
    //float intensity;
    int position; 
    int increment;

    /**
     *  Moving LED strip pattern to next state
     * @return whether the progress bar reach the top LED.
     */
    bool advance();
public:
    /**
     * Create a new progress bar. It starts at the 0 
     * end of the strip, moving up one grid every update
     */
    ProgressBar(const Color& bgColor, const Color& barColor);
    
    /**
     * Update the peak's position.
     * @return whether it reach the top point, 
     *  which is the (STRIP_LENGTH -1)th LED.
     */
    bool update();

    /**
     * Add progress bar's color to the strip, for output.
     *    
     */
    void apply(Color* stripColors);

    /**
     * Restart the Progress bar from the 0 end of the strip.
     */
    void restart();


};

LED_CONTROLLER_NAMESPACE_EXIT
