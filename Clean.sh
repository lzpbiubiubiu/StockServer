#!/bin/bash
find . -type d -name "GeneratedFiles" -print0 | xargs -0 rm -rf