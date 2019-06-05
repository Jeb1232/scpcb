#include <bbblitz3d.h>
#include <bbgraphics.h>

#include "Dreamfilter.h"
#include "../Config/Options.h"

namespace CBN {

// Globals.
MeshModel* ark_blur_image;
Texture* ark_blur_texture;
int ark_sw;
int ark_sh;
Camera* ark_blur_cam;

// Functions.
void CreateBlurImage() {
    //Create blur mainPlayer\cam
	Camera* cam = bbCreateCamera();
    bbCameraProjMode(cam,2);
    bbCameraZoom(cam,0.1f);
    bbCameraClsMode(cam, 0, 0);
    bbCameraRange(cam, 0.1f, 1.5f);
    bbMoveEntity(cam, 0, 0, 10000);
    ark_blur_cam = cam;

    ark_sw = userOptions->screenWidth;
    ark_sh = userOptions->screenHeight;
    bbCameraViewport(cam,0,0,ark_sw,ark_sh);

    //Create sprite
    MeshModel* spr = bbCreateMesh(cam);
    Surface* sf = bbCreateSurface(spr);
    bbAddVertex(sf, -1, 1, 0, 0, 0);
    bbAddVertex(sf, 1, 1, 0, 1, 0);
    bbAddVertex(sf, -1, -1, 0, 0, 1);
    bbAddVertex(sf, 1, -1, 0, 1, 1);
    bbAddTriangle(sf, 0, 1, 2);
    bbAddTriangle(sf, 3, 2, 1);
    bbEntityFX(spr, 17);
    bbScaleEntity(spr, 2048.f / (float)(ark_sw), 2048.f / (float)(ark_sw), 1);
    bbPositionEntity(spr, 0, 0, 1.0001f);
    bbEntityOrder(spr, -100000);
    bbEntityBlend(spr, 1);
    ark_blur_image = spr;

    //Create blur texture
    ark_blur_texture = bbCreateTexture(2048, 2048, 256);
    bbEntityTexture(spr, ark_blur_texture);
}

void UpdateBlur(float power) {

    bbEntityAlpha(ark_blur_image, power);

    //CopyRect(ark_sw / 2 - 1024, ark_sh / 2 - 1024, 2048, 2048, 0, 0, BackBuffer(), TextureBuffer(ark_blur_texture))
    bbCopyRect(0, 0, userOptions->screenWidth, userOptions->screenHeight, (int)(1024.f - userOptions->screenWidth / 2), (int)(1024.f - userOptions->screenHeight / 2), bbBackBuffer(), bbTextureBuffer(ark_blur_texture));

}

}
