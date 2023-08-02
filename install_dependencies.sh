# versions we snarf from package cloud
RMR_VER=4.8.1
# ARG SDL_VER=1.4.0
XFCPP_VER=2.3.6

# package cloud urls for wget
# Modified versione of the Dockerfile to install dependencies on bare metal

PC_REL_URL=https://packagecloud.io/o-ran-sc/release/packages/debian/stretch
# ARG PC_STG_URL=https://packagecloud.io/o-ran-sc/staging/packages/debian/stretch

# pull in rmr
wget -nv -nc --content-disposition ${PC_REL_URL}/rmr_${RMR_VER}_amd64.deb/download.deb
wget -nv -nc --content-disposition ${PC_REL_URL}/rmr-dev_${RMR_VER}_amd64.deb/download.deb
sudo dpkg -i rmr_${RMR_VER}_amd64.deb rmr-dev_${RMR_VER}_amd64.deb

# pull in xapp framework c++
wget -nv -nc --content-disposition ${PC_REL_URL}/ricxfcpp-dev_${XFCPP_VER}_amd64.deb/download.deb
wget -nv -nc --content-disposition ${PC_REL_URL}/ricxfcpp_${XFCPP_VER}_amd64.deb/download.deb
sudo dpkg -i ricxfcpp-dev_${XFCPP_VER}_amd64.deb ricxfcpp_${XFCPP_VER}_amd64.deb

sudo apt update
sudo apt install -y libcurl4-openssl-dev rapidjson-dev