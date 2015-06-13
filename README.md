# external-merge-sort
This program sorts big files using external memory. The algorithm uses the balanced interpolation based on k-ways and uses a Heap structure to make the interpolation step between the runs.

#How to compile
Just type the following command in the console to compile the source files and generated the initial unsorted files:
```
make
```
After that, type:
```
make tests
```
or run individual tests by typing:
```
./external-sort [source_file_path] [output_file_name] [memory_to_be_used_in_MB] [number_of_ways] 
```
