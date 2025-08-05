#!/bin/bash

chmod 000 no_www ;
make ;
./test_server_parser ;
make fclean ;
chmod 744 no_www ;
