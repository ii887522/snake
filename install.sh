python -m pip install virtualenv
virtualenv env
source env/bin/activate && python -m pip install cpplint && source env/bin/deactivate && docker run --rm --name snake_installer -w C:/snake -v $PWD:C:/snake snake-custom-node ncu.cmd -u && npm.cmd install
