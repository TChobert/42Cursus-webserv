#!/bin/bash

chmod 000 test_root_invalid ;
make ;
./test_server_parser ;
make fclean ;
chmod 744 test_root_invalid ;
