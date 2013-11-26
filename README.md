# PHP Extension load library

This extension allows load library.

## Build

```
% phpize
% ./configure
% make
% make install
```

## Configration

extension_load.ini:

```
extension=extension_load.so

;extension_load.dir=<DIR>
```

## Examples

```
extension_load('path/to/module.so');

//use module function/class
```

### ini file dir

extension_load.ini:

```
extension=extension_load.so

extension_load.dir=path/to/
```

```
//use module function/class
```
