#include "drawrend.h"
#include "svg.h"
#include "transforms.h"
#include "CGL/misc.h"
#include <iostream>
#include <sstream>
#include "CGL/lodepng.h"
#include "texture.h"
#include <ctime>
#include <time.h>
#include <cstdio>
#include <ctime>
using namespace std;

namespace CGL {

struct SVG;


DrawRend::~DrawRend( void ) {}

/**
* Initialize the renderer.
* Set default parameters and initialize the viewing transforms for each tab.
*/
void DrawRend::init() {
  gl = true;

  sample_rate = 1;
  left_clicked = false;
  show_zoom = 0;

  svg_to_ndc.resize(svgs.size());
  for (int i = 0; i < svgs.size(); ++i) {
    current_svg = i;
    view_init();
  }
  current_svg = 0;
  psm = P_NEAREST;
  lsm = L_ZERO;

}

/**
* Draw content.
* Simply reposts the framebuffer and the zoom window, if applicable.
*/
void DrawRend::render() {
  draw_pixels();
  if (show_zoom)
    draw_zoom();
}

/**
 * Respond to buffer resize.
 * Resizes the buffers and resets the
 * normalized device coords -> screen coords transform.
 * \param w The new width of the context
 * \param h The new height of the context
 */
void DrawRend::resize( size_t w, size_t h ) {
  width = w; height = h;

  framebuffer.resize(4 * w * h);

  samplebuffer.clear();
  vector<SampleBuffer> samplebuffer_row(width, SampleBuffer(sqrt(sample_rate)));
  for (int i = 0; i < height; ++i)
    samplebuffer.push_back(samplebuffer_row);

  float scale = min(width, height);
  ndc_to_screen(0,0) = scale; ndc_to_screen(0,2) = (width  - scale) / 2;
  ndc_to_screen(1,1) = scale; ndc_to_screen(1,2) = (height - scale) / 2;

  redraw();
}

/**
 * Return a brief description of the renderer.
 * Displays current buffer resolution, sampling method, sampling rate.
 */
static const string level_strings[] = { "level zero", "nearest level", "bilinear level interpolation"};
static const string pixel_strings[] = { "nearest pixel", "bilinear pixel interpolation"};
std::string DrawRend::info() {
  stringstream ss;
  stringstream sample_method;
  sample_method  << pixel_strings[psm];
  ss << "Resolution " << width << " x " << height << ". ";
  ss << "Using " << sample_method.str() << " sampling. ";
  ss << "Supersample rate " << sample_rate << " per pixel. ";
  if(toggle_scanline) ss << "Scan line";
  return ss.str();
}

/**
 * Respond to cursor events.
 * The viewer itself does not really care about the cursor but it will take
 * the GLFW cursor events and forward the ones that matter to  the renderer.
 * The arguments are defined in screen space coordinates ( (0,0) at top
 * left corner of the window and (w,h) at the bottom right corner.
 * \param x the x coordinate of the cursor
 * \param y the y coordinate of the cursor
 */
void DrawRend::cursor_event( float x, float y ) {
  // translate when left mouse button is held down
  if (left_clicked) {
    float dx = (x - cursor_x) / width  * svgs[current_svg]->width;
    float dy = (y - cursor_y) / height * svgs[current_svg]->height;
    move_view(dx,dy,1);
    redraw();
  }

  // register new cursor location
  cursor_x = x;
  cursor_y = y;
}

/**
 * Respond to zoom event.
 * Like cursor events, the viewer itself does not care about the mouse wheel
 * either, but it will take the GLFW wheel events and forward them directly
 * to the renderer.
 * \param offset_x Scroll offset in x direction
 * \param offset_y Scroll offset in y direction
 */
void DrawRend::scroll_event( float offset_x, float offset_y ) {
  if (offset_x || offset_y) {
    float scale = 1 + 0.05 * (offset_x + offset_y);
    scale = std::min(1.5f,std::max(0.5f,scale));
    move_view(0,0,scale);
    redraw();
  }
}

/**
 * Respond to mouse click event.
 * The viewer will always forward mouse click events to the renderer.
 * \param key The key that spawned the event. The mapping between the
 *        key values and the mouse buttons are given by the macros defined
 *        at the top of this file.
 * \param event The type of event. Possible values are 0, 1 and 2, which
 *        corresponds to the events defined in macros.
 * \param mods if any modifier keys are held down at the time of the event
 *        modifiers are defined in macros.
 */
void DrawRend::mouse_event( int key, int event, unsigned char mods ) {
  if (key == MOUSE_LEFT) {
    if (event == EVENT_PRESS)
      left_clicked = true;
    if (event == EVENT_RELEASE)
      left_clicked = false;
  }
}

/**
 * Respond to keyboard event.
 * The viewer will always forward mouse key events to the renderer.
 * \param key The key that spawned the event. ASCII numbers are used for
 *        letter characters. Non-letter keys are selectively supported
 *        and are defined in macros.
 * \param event The type of event. Possible values are 0, 1 and 2, which
 *        corresponds to the events defined in macros.
 * \param mods if any modifier keys are held down at the time of the event
 *        modifiers are defined in macros.
 */
void DrawRend::keyboard_event( int key, int event, unsigned char mods ) {
  if (event != EVENT_PRESS)
    return;

  // tab through the loaded files
  if (key >= '1' && key <= '9' && key-'1' < svgs.size()) {
    current_svg = key - '1';
    redraw();
    return;
  }

  switch( key ) {

    // reset view transformation
    case ' ':
      view_init();
      redraw();
      break;

    // set the sampling rate to 1, 4, 9, or 16
    case '=':
      if (sample_rate < 16) {
        sample_rate = (int)(sqrt(sample_rate)+1)*(sqrt(sample_rate)+1);

        samplebuffer.clear();
        vector<SampleBuffer> samplebuffer_row(width, SampleBuffer(sqrt(sample_rate)));
        for (int i = 0; i < height; ++i)
          samplebuffer.push_back(samplebuffer_row);
        redraw();
      }
      break;
    case '-':
      if (sample_rate > 1) {
        sample_rate = (int)(sqrt(sample_rate)-1)*(sqrt(sample_rate)-1);

        samplebuffer.clear();
        vector<SampleBuffer> samplebuffer_row(width, SampleBuffer(sqrt(sample_rate)));
        for (int i = 0; i < height; ++i)
          samplebuffer.push_back(samplebuffer_row);
        redraw();
      }
      break;

    // save the current buffer to disk
    case 'S':
      write_screenshot();
      break;

    // toggle pixel sampling scheme
    case 'P':
      psm = (PixelSampleMethod)((psm+1)%2);
      redraw();
      break;

    // // toggle level sampling scheme
    // case 'L':
    //   lsm = (LevelSampleMethod)((lsm+1)%3);
    //   redraw();
    //   break;

      // toggle scanline
    case 'L':
      toggle_scanline = !toggle_scanline;
      redraw();
      break;
    // toggle zoom
    case 'Z':
      show_zoom = (show_zoom+1)%2;
      break;

    default:
      return;
  }
}

/**
 * Writes the contents of the pixel buffer to disk as a .png file.
 * The image filename contains the month, date, hour, minute, and second
 * to make sure it is unique and identifiable.
 */
void DrawRend::write_screenshot() {
    redraw();
    if (show_zoom) draw_zoom();

    vector<unsigned char> windowPixels( 4*width*height );
    glReadPixels(0, 0,
                width,
                height,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                &windowPixels[0] );

    vector<unsigned char> flippedPixels( 4*width*height );
    for (int row = 0; row < height; ++row)
      memcpy(&flippedPixels[row * width * 4], &windowPixels[(height - row - 1) * width * 4], 4*width);

    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    stringstream ss;
    ss << "screenshot_" << lt->tm_mon+1 << "-" << lt->tm_mday << "_"
      << lt->tm_hour << "-" << lt->tm_min << "-" << lt->tm_sec << ".png";
    string file = ss.str();
    cout << "Writing file " << file << "...";
    if (lodepng::encode(file, flippedPixels, width, height))
      cerr << "Could not be written" << endl;
    else
      cout << "Success!" << endl;
}

/**
 * Writes the contents of the framebuffer to disk as a .png file.
 *
 */
void DrawRend::write_framebuffer() {
  if (lodepng::encode("test.png", &framebuffer[0], width, height))
    cerr << "Could not write framebuffer" << endl;
  else
    cerr << "Succesfully wrote framebuffer" << endl;
}


/**
 * Draws the current SVG tab to the screen. Also draws a
 * border around the SVG canvas. Resolves the supersample buffers
 * into the framebuffer before posting the framebuffer pixels to the screen.
 */
void DrawRend::redraw() {


  for (int i = 0; i < samplebuffer.size(); ++i)
    for (int j = 0; j < samplebuffer[i].size(); ++j)
      samplebuffer[i][j].clear();

  // clock time
  std::clock_t start;
  double duration;
  start = std::clock();

  SVG &svg = *svgs[current_svg];
  svg.draw(this, ndc_to_screen*svg_to_ndc[current_svg]);

  // draw canvas outline
  Vector2D a = ndc_to_screen*svg_to_ndc[current_svg]*(Vector2D(    0    ,     0    )); a.x--; a.y++;
  Vector2D b = ndc_to_screen*svg_to_ndc[current_svg]*(Vector2D(svg.width,     0    )); b.x++; b.y++;
  Vector2D c = ndc_to_screen*svg_to_ndc[current_svg]*(Vector2D(    0    ,svg.height)); c.x--; c.y--;
  Vector2D d = ndc_to_screen*svg_to_ndc[current_svg]*(Vector2D(svg.width,svg.height)); d.x++; d.y--;

  rasterize_line(a.x, a.y, b.x, b.y, Color::Black);
  rasterize_line(a.x, a.y, c.x, c.y, Color::Black);
  rasterize_line(d.x, d.y, b.x, b.y, Color::Black);
  rasterize_line(d.x, d.y, c.x, c.y, Color::Black);

  resolve();
  // end clock time
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<<"\ntime: "<< duration;

  if (gl)
    draw_pixels();
}

/**
 * OpenGL boilerplate to put an array of RGBA pixels on the screen.
 */
void DrawRend::draw_pixels() {
  const unsigned char *pixels = &framebuffer[0];
  // copy pixels to the screen
  glPushAttrib( GL_VIEWPORT_BIT );
  glViewport(0, 0, width, height);

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glOrtho( 0, width, 0, height, 0, 0 );

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glTranslatef( -1, 1, 0 );

  glRasterPos2f(0, 0);
  glPixelZoom( 1.0, -1.0 );
  glDrawPixels( width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
  glPixelZoom( 1.0, 1.0 );

  glPopAttrib();
  glMatrixMode( GL_PROJECTION ); glPopMatrix();
  glMatrixMode( GL_MODELVIEW  ); glPopMatrix();
}

/**
 * Reads off the pixels that should be in the zoom window, and
 * generates a pixel array with the zoomed view.
 */
void DrawRend::draw_zoom() {

  // size (in pixels) of region of interest
  size_t regionSize = 32;

  // relative size of zoom window
  size_t zoomFactor = 16;

  // compute zoom factor---the zoom window should never cover
  // more than 40% of the framebuffer, horizontally or vertically
  size_t bufferSize = min( width, height );
  if( regionSize*zoomFactor > bufferSize * 0.4) {
    zoomFactor = (bufferSize * 0.4 )/regionSize;
  }
  size_t zoomSize = regionSize * zoomFactor;

  // adjust the cursor coordinates so that the region of
  // interest never goes outside the bounds of the framebuffer
  size_t cX = max( regionSize/2, min( width-regionSize/2-1, (size_t) cursor_x ));
  size_t cY = max( regionSize/2, min( height-regionSize/2-1, height - (size_t) cursor_y ));

  // grab pixels from the region of interest
  vector<unsigned char> windowPixels( 3*regionSize*regionSize );
  glReadPixels( cX - regionSize/2,
                cY - regionSize/2 + 1, // meh
                regionSize,
                regionSize,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                &windowPixels[0] );

  // upsample by the zoom factor, highlighting pixel boundaries
  vector<unsigned char> zoomPixels( 3*zoomSize*zoomSize );
  unsigned char* wp = &windowPixels[0];
  // outer loop over pixels in region of interest
  for( int y = 0; y < regionSize; y++ ) {
   int y0 = y*zoomFactor;
   for( int x = 0; x < regionSize; x++ ) {
      int x0 = x*zoomFactor;
      unsigned char* zp = &zoomPixels[ ( x0 + y0*zoomSize )*3 ];
      // inner loop over upsampled block
      for( int j = 0; j < zoomFactor; j++ ) {
        for( int i = 0; i < zoomFactor; i++ ) {
          for( int k = 0; k < 3; k++ ) {
            // highlight pixel boundaries
            if( i == 0 || j == 0 ) {
              const float s = .3;
              zp[k] = (int)( (1.-2.*s)*wp[k] + s*255. );
            } else {
              zp[k] = wp[k];
            }
          }
          zp += 3;
        }
        zp += 3*( zoomSize - zoomFactor );
      }
      wp += 3;
    }
  }

  // copy pixels to the screen using OpenGL
  glMatrixMode( GL_PROJECTION ); glPushMatrix(); glLoadIdentity(); glOrtho( 0, width, 0, height, 0.01, 1000. );
  glMatrixMode( GL_MODELVIEW  ); glPushMatrix(); glLoadIdentity(); glTranslated( 0., 0., -1. );

  glRasterPos2i( width-zoomSize, height-zoomSize );
  glDrawPixels( zoomSize, zoomSize, GL_RGB, GL_UNSIGNED_BYTE, &zoomPixels[0] );
  glMatrixMode( GL_PROJECTION ); glPopMatrix();
  glMatrixMode( GL_MODELVIEW ); glPopMatrix();

}

/**
 * Initializes the default viewport to center and reasonably zoom the SVG
 * with a bit of margin.
 */
void DrawRend::view_init() {
  float w = svgs[current_svg]->width, h = svgs[current_svg]->height;
  set_view(w/2, h/2, 1.2 * std::max(w,h) / 2);
}

/**
 * Sets the viewing transform matrix corresponding to a view centered at
 * (x,y) in SVG space, extending 'span' units in all four directions.
 * This transform maps to 'normalized device coordinates' (ndc), where the window
 * corresponds to the [0,1]^2 rectangle.
 */
void DrawRend::set_view(float x, float y, float span) {
  svg_to_ndc[current_svg] = Matrix3x3(1,0,-x+span,  0,1,-y+span,  0,0,2*span);
}

// toggle between scanline and classical rasterization
void DrawRend::rasterize_fig( float x0, float y0,
                         float x1, float y1,
                         float x2, float y2,
                         Color color, Triangle *tri) {
                           if(!toggle_scanline) rasterize_triangle(x0,y0,x1,y1,x2,y2,color,tri);
                           else rasterize_scanline(x0,y0,x1,y1,x2,y2,color,tri);
                         }
  
  void DrawRend::rasterize_scanline( float x0, float y0,
                        float x11, float y1,
                        float x22, float y2,
                        Color color, Triangle *tri) {

  vector<vector<float>> activeEdgeTable;
  vector<vector<float>> edgeTable;

}

/**
 * Recovers the previous viewing center and span from the viewing matrix,
 * then shifts and zooms the viewing window by setting a new view matrix.
 */
void DrawRend::move_view(float dx, float dy, float zoom) {
  Matrix3x3& m = svg_to_ndc[current_svg];
  float span = m(2,2)/2.;
  float x = span - m(0,2), y = span - m(1,2);
  set_view(x - dx, y - dy, span * zoom);
}



///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Coding Section Begins Here/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


// Part1: Rasterize a point.
void DrawRend::rasterize_point( float x, float y, Color color ) {

int floor_x = floor(x);
  int floor_y = floor(y);
  
  if(floor_x < width && floor_y < height) samplebuffer[floor_y][floor_x].fill_pixel(color);

}

// Part2: Rasterize a line.
void DrawRend::rasterize_line( float x0, float y0,
                     float x1, float y1,
                     Color color) {

float dx = x1-x0;
  float dy = y1-y0;
  float slope = dy/dx;

  //rasterize start and end point
  rasterize_point(x0,y0,color);
  rasterize_point(x1,y1,color);

  if(slope > 0){
    //positive slope
    if(slope > 1){
      //dy > dx
      int steps = floor(abs(dy));
      for(int i=0;i<steps-1;i++){
        if(dx < 0 && dy < 0){
          //both negative
          x1+=abs(dx)/steps;
          y1+=abs(dy)/steps;
          rasterize_point(x1,y1,color);
        }
        else{
          x0+=abs(dx)/steps;
          y0+=abs(dy)/steps;
          rasterize_point(x0,y0,color);
        }
      }
    }
    else{
      //dx > dy
      int steps = floor(abs(dx));
      for(int i=0;i<steps-1;i++){
        if(dx < 0 && dy < 0){
          //both negative
          x1+=abs(dx)/steps;
          y1+=abs(dy)/steps;
          rasterize_point(x1,y1,color);
        }
        else{
          x0+=abs(dx)/steps;
          y0+=abs(dy)/steps;
          rasterize_point(x0,y0,color);
        }
      }

    }
  }
  else if(slope < 0){
    //negative slope
    int steps = abs(dx) > abs(dy) ? floor(abs(dx)):floor(abs(dy));
    if(dx < 0){
       for(int i=0;i<steps-1;i++){
          x1+=abs(dx)/steps;
          y1=y1-abs(dy)/steps;
          rasterize_point(x1,y1,color);
       }

    }
    else{
      for(int i=0;i<steps-1;i++){
          x1=x1-abs(dx)/steps;
          y1=y1+abs(dy)/steps;
          rasterize_point(x1,y1,color);
       }

    }
  }
  else{
    //slope == 0
    int steps = floor(abs(dx));
    for(int i=0;i<steps-1;i++){
        if(x1 > x0){x0+=dx/steps;rasterize_point(x0,y0,color);}
        else{x1+=abs(dx)/steps;rasterize_point(x1,y1,color);}
    }
  }

}
 
//Part 4: Calculate Dot product
float DrawRend::dot_product(float x0, float y0,float x1, float y1, float xp, float yp){
  // (x0,y0), (x1,y1) are the endpoints of the edge. (xp,yp) is the point.

  float tan_x = x1-x0;
  float tan_y = y1-y0;

  float tan_px = xp-x0;
  float tan_py = yp-y0;


  float perp[]={-tan_y,tan_x};

  return perp[0]*tan_px + perp[1]*tan_py;


}

//Part3,5,7: Rasterize a triangle.
void DrawRend::rasterize_triangle( float x0, float y0,
                        float x1, float y1,
                        float x2, float y2,
                        Color color, Triangle *tri) {

//Part3: Create a 2D loop to fill the bounding boxes.

//Part5: Modify your loop to fill only the pixels inside the triangles.

//Part7: Modify your loop to fill every sub_pixel inside the trianlges.

float min_x , min_y, max_x,max_y;

min_x = x0 < x1 ? (x0<x2?x0:x2):(x1<x2?x1:x2);
min_y = y0 < y1 ? (y0<y2?y0:y2):(y1<y2?y1:y2);

max_x = x0 > x1 ? (x0>x2?x0:x2):(x1>x2?x1:x2);
max_y = y0 > y1 ? (y0>y2?y0:y2):(y1>y2?y1:y2);

//travering through pixel centers
// for(float i=min_x;i<max_x;i+=0.5){
//   for(float j=min_y;j<max_y;j+=0.5){
//       if((dot_product(x0,y0,x1,y1,i,j) >= 0 && dot_product(x1,y1,x2,y2,i,j)>=0 && dot_product(x2,y2,x0,y0,i,j) >=0) ||
//          (dot_product(x1,y1,x0,y0,i,j) >=0 && dot_product(x2,y2,x1,y1,i,j)>=0 && dot_product(x0,y0,x2,y2,i,j)>=0))    
//         rasterize_point(i,j,color); 
//   }
// } 

//antialiasing via supersampling

float sub_pixsize=1/sqrt(sample_rate*2);

for(float i=min_x+sub_pixsize;i<max_x;i+=0.25){
  for(float j=min_y+sub_pixsize;j<max_y;j+=0.25){
      //subpixels loop
      for(float k=0;k<sqrt(sample_rate);k++){
        for(float l=0;l<sqrt(sample_rate);l++){
          
          float row_center =    i + k/sqrt(sample_rate);  //adding row and column pixel offset to subpixels 
          float column_center = j + l/sqrt(sample_rate);

          if((dot_product(x0,y0,x1,y1,row_center,column_center) >= 0 && dot_product(x1,y1,x2,y2,row_center,column_center)>=0 && dot_product(x2,y2,x0,y0,row_center,column_center) >=0) ||
        (dot_product(x1,y1,x0,y0,row_center,column_center) >=0 && dot_product(x2,y2,x1,y1,row_center,column_center)>=0 && dot_product(x0,y0,x2,y2,row_center,column_center)>=0))    
                  samplebuffer[j][i].fill_color(k,l,color);
        }
      }
  }
}







 
}



}
