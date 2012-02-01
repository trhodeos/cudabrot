#include "os_gl.h"

class Cudabrot {
public:
  Cudabrot(unsigned int image_width, unsigned int image_height);
  ~Cudabrot();

  void Run();
  void Draw();

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
};
