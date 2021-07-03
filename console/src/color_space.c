// color_space.c

//
// Color conversion support functions 
// rgb -> LAB, and finding best color matches
//
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "logging.h"

// need to link in math.h '-lm' for sqrt(), pow()
#include "color_space.h"


// Test a list of colors for a best possible LAB match
// Returns true if successful, index of match color will be in: p_match_id
// Expects LAB data to be populated for list of colors
bool color_find_closest_LAB(palette_rgb_LAB * color_list, color_rgb_LAB * p_match_color, uint8_t * p_match_id) {

    int color_distance;
    int color_distance_min;

    if (color_list->color_count <= 0)
        return false;

    // Prime loop with first value
    color_distance_min = color_distance = color_distance_LAB_CIE76(&(color_list->colors[0]), p_match_color);
    *p_match_id = 0;

    int c;
    for (c = 1; c < color_list->color_count; c++) {

        color_distance = color_distance_LAB_CIE76(&(color_list->colors[c]), p_match_color);

        // Update if match is closest yet
        if (color_distance < color_distance_min) {
            *p_match_id = c;
            color_distance_min = color_distance;
        }
    }

    return true;
}


// Test a list of colors for a perfect RGB match
// Returns true if successful, index of match color will be in: p_match_id
bool color_find_exact_RGB(palette_rgb_LAB * color_list, color_rgb_LAB * p_match_color, uint8_t * p_match_id) {

    int c;
    for (c = 0; c < color_list->color_count; c++) {

//printf(" %d: exactrgb: rgb(%02x,%02x,%02x) vs rgb(%02x,%02x,%02x)\n", c,
            // color_list->colors[c].r, color_list->colors[c].g, color_list->colors[c].b,
            // p_match_color->r, p_match_color->g, p_match_color->b);

        if ((color_list->colors[c].r == p_match_color->r) &&
            (color_list->colors[c].g == p_match_color->g) &&
            (color_list->colors[c].b == p_match_color->b)) {

//printf("yes\n");
            *p_match_id = c;
            return true;
        }
    }
    return false;
}


// Based on www.easyrgb.com
// Convert uint8_t RGB (0 - 255) to double LAB (0 - 100, -128 - 128, -128 - 128)
// Accepts pointer to color entry, the return values are stored in it's (.L, .A, .B) vars
void color_rgb2LAB(color_rgb_LAB * color) {

    // Convert RGB range from 0-255 to 0.0 to 1.0    
    double r = (double)color->r / 255.0,
           g = (double)color->g / 255.0,
           b = (double)color->b / 255.0;
    double x, y, z;

    // RGB -> XYZ
    r = (r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) : r / 12.92;
    g = (g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) : g / 12.92;
    b = (b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) : b / 12.92;

    // For standard XYZ output, these need to be scaled up.
    // But since this is going straight to LAB, they would
    // just get scaled back down again.
    // // r *= 100.0;
    // // g *= 100.0;
    // // b *= 100.0;

    x = r * 0.4124 + g * 0.3576 + b * 0.1805;
    y = r * 0.2126 + g * 0.7152 + b * 0.0722;
    z = r * 0.0193 + g * 0.1192 + b * 0.9505;

    // Color reference adjustment
    x /= 0.95047;
    y /= 1.00000;
    z /= 1.08883;

    // See scaling note about r,g,b above
    // // x /= 100.0;
    // // y /= 100.0;
    // // z /= 100.0;

    // XYZ -> LAB
    x = (x > 0.008856) ? pow(x, 1.0/3.0) : (7.787 * x) + 16.0/116.0;
    y = (y > 0.008856) ? pow(y, 1.0/3.0) : (7.787 * y) + 16.0/116.0;
    z = (z > 0.008856) ? pow(z, 1.0/3.0) : (7.787 * z) + 16.0/116.0;

    // Final conversion and store values
    color->L = (116.0 * y) - 16.0;
    color->A = 500.0 * (x - y);
    color->B = 200.0 * (y - z);
}


// Based on www.easyrgb.com
// Calculate distance between two colors in LAB format using: Delta E CIE76
double color_distance_LAB_CIE76(color_rgb_LAB * color_A, color_rgb_LAB * color_B) {

    return sqrt( 
                ( (color_A->L - color_B->L) * (color_A->L - color_B->L) ) +
                ( (color_A->A - color_B->A) * (color_A->A - color_B->A) ) + 
                ( (color_A->B - color_B->B) * (color_A->B - color_B->B) ) );
}


// int main () {
//     color_rgb_LAB colA, colB;

//     colA.r = 255;
//     colA.r = 128;
//     colA.r = 0;
//     color_rgb2LAB(&colA);

//     colB.r = 0;
//     colB.r = 255;
//     colB.r = 128;
//     color_rgb2LAB(&colB);

//     printf("%f \n", color_distance_LAB_CIE76(&colA, &colB));
// }


/*
Delta E CIE94

CIE-L*1, CIE-a*1, CIE-b*1          //Color #1 CIE-L*ab values
CIE-L*2, CIE-a*2, CIE-b*2          //Color #2 CIE-L*ab values
WHT-L, WHT-C, WHT-H                //Weighting factors

xC1 = sqrt( ( CIE-a*1 ^ 2 ) + ( CIE-b*1 ^ 2 ) )
xC2 = sqrt( ( CIE-a*2 ^ 2 ) + ( CIE-b*2 ^ 2 ) )
xDL = CIE-L*2 - CIE-L*1
xDC = xC2 - xC1
xDE = sqrt( ( ( CIE-L*1 - CIE-L*2 ) * ( CIE-L*1 - CIE-L*2 ) )
          + ( ( CIE-a*1 - CIE-a*2 ) * ( CIE-a*1 - CIE-a*2 ) )
          + ( ( CIE-b*1 - CIE-b*2 ) * ( CIE-b*1 - CIE-b*2 ) ) )

xDH = ( xDE * xDE ) - ( xDL * xDL ) - ( xDC * xDC )
if ( xDH > 0 )
{
   xDH = sqrt( xDH )
}
else
{
   xDH = 0
}
xSC = 1 + ( 0.045 * xC1 )
xSH = 1 + ( 0.015 * xC1 )
xDL /= WHT-L
xDC /= WHT-C * xSC
xDH /= WHT-H * xSH

Delta E94 = sqrt( xDL ^ 2 + xDC ^ 2 + xDH ^ 2 )
*/