# fps6-builder
This application bundles several asset files into a single archive to be used by [fps6](https://github.com/falki147/fps6). The format resembles the [GTA IMG archive](https://gtamods.com/wiki/IMG_archive), specifically the version GTA SA uses.

## Building
Building the application is very straightforward, since it's a sigle CPP file with no dependencies. For building it cmake is used. Just run cmake and build it, no additional configuration required.

## Usage
Make sure the binary built before is either in the build folder or in the project root folder. There's a helper script provided (`build-img.bat`) which combines all the files in the assets directory into one archive using this application.
