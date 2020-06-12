# Rom source code: https://github.com/Evolution-X

# Git lfs 
git lfs install 

# Make clean
sudo rm -r out

# Set up environment
. build/envsetup.sh

# Choose a target
lunch aosp_oneplus3-userdebug

# Build the code
mka bacon -j4
