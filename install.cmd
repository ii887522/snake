python -m pip install virtualenv
virtualenv env
env\Scripts\activate && python -m pip install cpplint && env\Scripts\deactivate && docker run --rm --name snake_installer -w C:\snake -v %CD%:C:\snake snake-custom-node ncu.cmd -u && npm.cmd install
