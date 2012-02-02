#include "os_gl.h"

class Cudabrot {
public:
  Cudabrot(unsigned int image_width, unsigned int image_height);
  ~Cudabrot();

  void Run();
  void Draw();

  void Zoom(unsigned int new_cx, unsigned int new_cy);
  void UnZoom(unsigned int new_cx, unsigned int new_cy);

private:
  void CreateTexture();
  void DeleteTexture();

  void CreatePBO();
  void DeletePBO();

  void LaunchKernel(unsigned char* buffer);

private:
  unsigned int image_width_;
  unsigned int image_height_;

  GLuint texture_id_;
  GLuint pbo_;

  float cx_;
  float cy_;
  float scale_;
};
