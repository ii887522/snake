'use strict'

import { options, dependencies, zip, dll } from '@ii887522/packify'

options.outDirPath = 'libs/'
options.x86DllOutDirPaths = ['snake/Debug/', 'snake/Release/', 'snake/Test/'],
options.x64DllOutDirPaths = ['snake/x64/Debug/', 'snake/x64/Release/', 'snake/x64/Test/']
const accessToken = '<access-token>'

dependencies(async () => {
  await Promise.all([
    zip('https://www.libsdl.org/release/SDL2-devel-2.0.12-VC.zip'),
    zip('https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-VC.zip'),
    zip('https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.15-VC.zip'),
    zip('https://gitlab.com/api/v4/projects/23530641/packages/generic/nitro/1.0.2/nitro-1.0.2.zip', { 'PRIVATE-TOKEN': accessToken }),
    zip('https://gitlab.com/api/v4/projects/23074770/packages/generic/viewify/1.0.2/viewify-1.0.2.zip', { 'PRIVATE-TOKEN': accessToken })
  ])
  dll('x86', 'SDL2-2.0.12/lib/x86/SDL2.dll')
  dll('x64', 'SDL2-2.0.12/lib/X64/SDL2.dll')
  dll('x86', 'SDL2_image-2.0.5/lib/x86/libpng16-16.dll')
  dll('x86', 'SDL2_image-2.0.5/lib/x86/SDL2_image.dll')
  dll('x86', 'SDL2_image-2.0.5/lib/x86/zlib1.dll')
  dll('x64', 'SDL2_image-2.0.5/lib/x64/libpng16-16.dll')
  dll('x64', 'SDL2_image-2.0.5/lib/x64/SDL2_image.dll')
  dll('x64', 'SDL2_image-2.0.5/lib/x64/zlib1.dll')
  dll('x86', 'SDL2_ttf-2.0.15/lib/x86/libfreetype-6.dll')
  dll('x86', 'SDL2_ttf-2.0.15/lib/x86/SDL2_ttf.dll')
  dll('x64', 'SDL2_ttf-2.0.15/lib/x64/libfreetype-6.dll')
  dll('x64', 'SDL2_ttf-2.0.15/lib/x64/SDL2_ttf.dll')
})
