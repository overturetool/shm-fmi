#!/bin/bash
kill -9 $(cat server.pid)
rm server.pid
