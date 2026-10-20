#include <stdio.h>
#include <string.h>
#include <math.h>


#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

#define pi 3.141592
const int screen_width = 80;
const int screen_height = 22;

const float theta_spacing = 0.07;
const float phi_spacing = 0.02;
const float R1 = 1;
const float R2 = 2;
const float K2 = 10000;
#define K1 (screen_width*K2*3/(8*(R1+R2)))

void render_frame(float A, float B)
{
    float cosA = cos(A), sinA = sin(A);
    float cosB = cos(B), sinB = sin(B);
    char output[screen_width * screen_height];
    float zbuffer[screen_width * screen_height];
    
    memset(output, ' ', screen_height * screen_width);
    memset(zbuffer, 0, screen_height * screen_width * sizeof(float));

    for(float theta = 0; theta < 2 * pi; theta += theta_spacing)
    {
        float costheta = cos(theta), sintheta = sin(theta);
        for(float phi = 0; phi < 2 * pi; phi += phi_spacing)
        {
            float cosphi = cos(phi), sinphi = sin(phi);
            float cx = R2 + R1 * costheta;
            float cy = R1 * sintheta;
            float X = cx * (cosB * cosphi + sinA * sinB * sinphi) - cy * cosA * sinB;
            float Y = cx * (sinB * cosphi - sinA * cosB * sinphi) + cy * cosA * cosB;
            float z = K2 + cosA * cx * sinphi + cy * sinA;
            float ooz = 1 / z;
            int xp = (int)(screen_width / 2 + K1 * ooz * X);
            int yp = (int)(screen_height / 2 - K1 * ooz * Y *0.4);
            
            // Calculate luminance
            float L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);

            if(L > 0)
            {
                if(xp >= 0 && xp < screen_width && yp >= 0 && yp < screen_height)
                {
                    int idx = xp + yp * screen_width;

                    if(ooz > zbuffer[idx])
                    {
                        zbuffer[idx] = ooz;
                        int luminance_index = (int)(L * 8);
                        // Cap the index to 11 to prevent potential out-of-bounds array access
                        if(luminance_index > 11) luminance_index = 11;
                        output[idx] = ".,-~;:=!*#$@"[luminance_index];
                    }
                }
            }
        }
    }
    
 
    printf("\x1b[H");
    
    for(int j = 0; j < screen_height; j++)
    {
        for(int i = 0; i < screen_width; i++)
        {
            putchar(output[i + j * screen_width]);
        }
        putchar('\n');
    }
}

int main() {
    float A = 0, B = 0;
    
    // Clear screen once
    printf("\x1b[2J");
    
    for(;;) {
        render_frame(A, B);
        A += 0.08;
        B += 0.03;
        
        // Sleep for 30 milliseconds instead of 3 seconds
        SLEEP_MS(0.001); 
    }
    return 0;
}
