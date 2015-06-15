//
//  binary-file-manager.h
//  merge-sort-external
//
//  Created by Diego Falcão on 5/31/15.
//  Copyright (c) 2015 Diego Falcão. All rights reserved.
//

#ifndef __binary_file_manager_h__
#define __binary_file_manager_h__

int32_t write_binary_file (char *file_name, long total_size);
int32_t read_binary_file (char *file_name);

#endif