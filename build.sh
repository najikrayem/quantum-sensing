echo "Starting build process..."
echo ""

# Navigate to the build directory. Make it if it doesn't exist.
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build || { echo "Failed to create build directory. Exiting."; exit 1; }
else
    echo "Build directory already exists."
fi
echo "Navigating to the build directory..."
cd build || { echo "Failed to navigate to the build directory. Exiting."; exit 1; }
echo ""


# Run CMake configuration
echo "Configuring the project with CMake..."
cmake -DPICO_SDK_PATH=~/pico-sdk -DPICO_BOARD=pico2 ..
echo "CMake configuration completed."
echo ""


# Build the project
# determine the number of available CPU cores
num_cores=$(nproc --all)
# build the project using the determined number of threads
echo "Building the project with make (using $num_cores threads)..."
make -j$num_cores | pv -l > /dev/null
echo "Build completed successfully."
echo ""


# Navigate back to the root directory
echo "Returning to the root directory..."
cd .. || { echo "Failed to return to the root directory. Exiting."; exit 1; }
echo ""

# Load the program
# echo "Loading the program onto the board..."
# sh upload.sh
# echo ""


echo "Build and load process completed successfully!"
echo ""