Hello, project partner!

I have added a lot of shit to the repository. :3 That's what I do!

So here's some things that are needed for this:

* OFX : openFrameworks. Get 'er downloaded from openframeworks.cc and just put the library any old place.
  * ofxCv : this is an addon for OFX that doesn't come installed with it. You'll need to go to the "addons" directory in your downloaded OpenFrameworks library folder thingy, and then `git clone https://github.com/kylemcdonald/ofxCv`. To add it to your project, use the ProjectGenerator that ships with OFX to add it, and then you also have to go into your project.xcodeproj file and change the HEADER_SEARCH_PATHS line to `HEADER_SEARCH_PATHS = $(OF_CORE_HEADERS) "../../../addons/ofxOpenCv/libs/opencv/include/" "../../../addons/ofxCv/libs/ofxCv/include/"`

* C++
  * I hate C++, you hate C++, but we all write in C++. Since we have to do JSON shit in C++, we can be a li'l sad or we can add in a library that does it for us. I am using nlohmann/json. Surprise! This ships as a part of OFX. So you literally don't have to do anything to make this work.
