export GYP_GENERATORS="ninja,xcode-ninja"
export GYP_GENERATOR_FLAGS="xcode_ninja_main_gyp=src/build/ninja/all.ninja.gyp"
export GYP_DEFINES="branding=Chromium buildtype=Official proprietary_codecs=1 ffmpeg_branding=Chrome"
# set asan=1 to enable memory check tool
#export GYP_DEFINES="asan=1 branding=Chromium buildtype=Official proprietary_codecs=1 ffmpeg_branding=Chrome"
