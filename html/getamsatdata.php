<?php
/*
    the data are sent by the C program "amsatdisplay" to socket 46972
    read this data and send it back to the browser
*/

    $rdsocket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
    socket_set_option($rdsocket, SOL_SOCKET, SO_REUSEADDR, 1);
    socket_bind($rdsocket, '127.0.0.1', 46972);

    // request one line from amsatdisplay.c
    $wrsocket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
    socket_set_option($wrsocket, SOL_SOCKET, SO_REUSEADDR, 1);
    socket_sendto($wrsocket, "REQLINE",7, 0, '127.0.0.1', 46973);
    
    // read the answer
    echo socket_read ($rdsocket, 1024);
    
    socket_close($rdsocket);
    socket_close($wrsocket);
?>
