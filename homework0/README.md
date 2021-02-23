# HW0_Synthetic_Lighting

![Markdown Logo](./manual_images/four.jpg?)


### Part 1: Setup [5 marks]

Find a setting with four different light sources. Now click four images using the following scheme:

  - The camera location should remain **exactly** the same throughout the process.
  - Each image should have exactly 1 light source turned on.
  - The four images should be identical in terms of size.
  - Images should be in jpg format (if not, you need to manually change the expected format in main.cpp)
 
 Rename the images to `im1.jpg`, `im2.jpg`, `im3.jpg`, `im4.jpg` and place them in the img directory.

**IMPORTANT: Once you've taken the four images, go to `main.cpp` and add the length and width of **your** images in the image_width and image_height variables (on the top).** You can check the size of an image by opening it's properties(right click).

![Markdown Logo](./manual_images/ss.png?)

In case you were unable to take images of your own, four images have already been placed in the `img` folder that you can use to implement the next part.

### Part2: Four-way Morphing [15 Marks]

![Markdown Logo](./img/im1.jpg?)
![Markdown Logo](./img/im2.jpg?)
![Markdown Logo](./img/im3.jpg?)
![Markdown Logo](./img/im4.jpg?)

Go to the `morphing4()` function in *morphing.cpp*. 

Given the four input images `inp1`,`inp2`,`inp3` and `inp4` and the horizontal and vertical weights `wh` and `wv`, use the concepts and techniques learnt in the lab to morph between the four light sources.

Try to extend the equation for 2-image interpolation to a 4-image interpolation yourself.

To test your implementation, run the following command:

	g++ main.cpp -o output `pkg-config --cflags --libs opencv4`;./output 0

Now move your cursor horizontally and vertically on the image. If implemented correctly, you will observe the transitions between the four lights as you move your cursor.

### Part3: Four-way Colored Morphing [10 Marks]

Go to the `filtered_morphing4()` function in *morphing.cpp*. 

Now implement a four-light version of colored_morphing yourself.

Given the four input images and two weights, change the color of the light sources to four different colors and perform 4-way morphing on the resulting images. 
You are encourged to use the funtions that you've already created (i.e. `colored_light()` and `morphing4()`).
	
To test your implementation, run the following command:

	g++ main.cpp -o output `pkg-config --cflags --libs opencv4`;./output 1

Now move your cursor horizontally and vertically on the image. If implemented correctly, you will observe the transitions between the four differently colored lights as you move your cursor.

## 
<p align="center">
    <img src="https://image-cdn.neatoshop.com/styleimg/31449/none/black/default/291607-19.jpg" width="400">
</p>

Resources used and further reads: 
- https://magnetiq.ca/pages/multiple-synthetic-lights/
- http://www.graficaobscura.com/synth/
