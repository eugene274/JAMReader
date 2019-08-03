# JAMReader

simple converter from output of JAM to ROOT

### Building

```
git clone https://github.com/eugene274/JAMReader.git
cd JAMReader
mkdir -v build; cd build
cmake -DROOT_DIR=$ROOTSYS/cmake ../
make
```

### Usage
To perform conversion, call
```
./JAMReader <path-to-input-file> <path-to-output-file>
```

Note JAM produces gunzipped text file.
