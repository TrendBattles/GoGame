mkdir build & cd build

cmake ..

cmake --build . --config Release

cd ../include/KataGo

:: Send "quit" into KataGo. 
:: It will start up, initialize the GPU (building the tuning data), and then immediately exit.
echo quit | katago.exe gtp -model model.bin.gz -config analysis.cfg