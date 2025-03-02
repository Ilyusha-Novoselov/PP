if [ ! -d "build" ]; then
  mkdir build
fi

cd build

cmake ..

cmake --build .

cd ./Lab_1

./Lab_1