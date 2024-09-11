#include <math.h>
#include <stdio.h>
#include <string.h>

// Donut parameters
// The viewer is located at the origin of the space, and the camera looks down the z axis (towards the negative values). 
// That way, the xy plane in 3d space aligns with the display?s xy coordinates.
const float R1=2, R2=1.25; //large and small radius of donut
const float ptDensity=50;
const int P1=R1*ptDensity, P2=R2*ptDensity; //number of points on each dimension of the surface
const float D=(R1+R2)*3; //distance from viewer

// Projection and display parameters
const float zN=1, zF=D+R1+R2; //clipping distances for matrix projection
const float screenWidth=1, screenHeight=1; //screen width and height
const int resolution=50;
const int W=screenWidth*resolution, H=screenHeight*resolution; //display width and height

// Lightsource parameters
const char* lightScale = ".,-~:;=!*#$@";
const int maxL = 12;
const float lVec[3] = {-12./13,4./13,3./13}; // vector has to be normalised to one then multiplied by the maximum light intensity

void renderFrame(float xRot, float yRot) // rotates donut around x and then around y axes (the donut is symmetric to rotation around the z axis)
{
	// Flush the console and reset the cursor(only works on *nix systems)
	printf("\x1B[2J\x1B[H");
	char display[H][W];
	float depth[H][W];
	memset(display, ' ', H*W);
	memset(depth, -zF, H*W*sizeof(float)); // anything behind zF is not displayed.
	for(int i=0; i<P1; ++i){
		for(int j=0; j<P2; ++j){
			// Calculate space coordinates and normal vector 
			float theta = 2*M_PI*i/P1, phi=2*M_PI*j/P2;
			float x=(cos(yRot)*cos(theta)-sin(yRot)*sin(xRot)*sin(theta))*(R1+cos(phi)*R2)+sin(yRot)*cos(xRot)*sin(phi)*R2;
			float y=cos(xRot)*sin(theta)*(R1+cos(phi)*R2)+sin(xRot)*sin(phi)*R2;
			float z=cos(yRot)*cos(xRot)*sin(phi)*R2-(cos(yRot)*sin(xRot)*sin(theta)+sin(yRot)*cos(theta))*(R1+cos(phi)*R2)-D;
			// The normal vector is just the spherical coordinates of the corresponding angles (rotated around the x and y axes) and thus has norm 1
			float norm[3] = {cos(yRot)*cos(theta)*cos(phi)+sin(yRot)*(cos(xRot)*sin(phi)-sin(xRot)*sin(theta)*cos(phi))
					,cos(xRot)*sin(theta)*cos(phi)+sin(xRot)*sin(phi)
					,cos(yRot)*(cos(xRot)*sin(phi)-sin(xRot)*sin(theta)*cos(phi))-sin(yRot)*cos(theta)*cos(phi)};

			// Calculate brightness (rounded down)
			// The cross product has to be reset to a (0,1) interval and then to a (0, maxL) interval
			int L = (lVec[0]*norm[0]+lVec[1]*norm[1]+lVec[2]*norm[2]+1)/2*maxL-0.5; // the 0.5 value is to avoid oversaturation and a having brightness out of range 

			// Map to display
			// The x coordinates are first scaled to fit in a (-W, +W) range, then normalised to (0,W). Same goes for y and H 
			int xd=(x*zN/z*resolution+W)/2, yd=(y*zN/z*resolution+H)/2;
			if(xd>0 && xd<H && yd>0 && yd<W && z>depth[yd][xd]){// only display if in range and if there isn't another pixel closer to the camera.
				display[yd][xd]=lightScale[L];
				depth[yd][xd]=z;
			}
		}
	}
	for(int i=0; i<H; ++i){
		for(int j=0; j<W; ++j){
			printf("%c", display[i][j]);
		}
		printf("\n");
	}

}

int main()
{
	float xRot=0, yRot=0;
	while(1){
		xRot+=0.07;
		yRot+=0.1;
		renderFrame(xRot,yRot);
	}
}
