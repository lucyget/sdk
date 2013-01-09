// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "embedders/android/graphics.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "embedders/android/log.h"

Graphics::Graphics(android_app* application, Timer* timer)
    : application_(application),
      timer_(timer),
      width_(0),
      height_(0),
      display_(EGL_NO_DISPLAY),
      surface_(EGL_NO_SURFACE),
      context_(EGL_NO_CONTEXT) {
}

const int32_t& Graphics::height() {
  return height_;
}

const int32_t& Graphics::width() {
  return width_;
}

int32_t Graphics::Start() {
  EGLint format, numConfigs;
  EGLConfig config;
  const EGLint attributes[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_NONE
  };
  static const EGLint ctx_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };

  display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (display_ != EGL_NO_DISPLAY) {
    LOGI("eglInitialize");
    if (eglInitialize(display_, NULL, NULL)) {
      LOGI("eglChooseConfig");
      if (eglChooseConfig(display_, attributes, &config, 1, &numConfigs) &&
          numConfigs > 0) {
        LOGI("eglGetConfigAttrib");
        if (eglGetConfigAttrib(display_, config,
                               EGL_NATIVE_VISUAL_ID, &format)) {
          ANativeWindow_setBuffersGeometry(application_->window, 0, 0, format);
          surface_ = eglCreateWindowSurface(display_, config,
                              (EGLNativeWindowType)application_->window, NULL);
          if (surface_ != EGL_NO_SURFACE) {
            LOGI("eglCreateContext");
            context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT,
                                        ctx_attribs);
            if (context_ != EGL_NO_CONTEXT) {
              if (eglMakeCurrent(display_, surface_, surface_, context_) &&
                  eglQuerySurface(display_, surface_, EGL_WIDTH, &width_) &&
                  width_ > 0 &&
                  eglQuerySurface(display_, surface_, EGL_HEIGHT, &height_) &&
                  height_ > 0) {
                glViewport(0, 0, width_, height_);
                return 0;
              }
            }
          }
        }
      }
    }
  }
  LOGE("Error starting graphics");
  Stop();
  return -1;
}

void Graphics::Stop() {
  LOGI("Stopping graphics");
  if (display_ != EGL_NO_DISPLAY) {
    eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context_ != EGL_NO_CONTEXT) {
      eglDestroyContext(display_, context_);
      context_ = EGL_NO_CONTEXT;
    }
    if (surface_ != EGL_NO_SURFACE) {
      eglDestroySurface(display_, surface_);
      surface_ = EGL_NO_SURFACE;
    }
    eglTerminate(display_);
    display_ = EGL_NO_DISPLAY;
  }
}

int32_t Graphics::Update() {
  return 0;
}
