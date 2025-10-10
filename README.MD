### Radar Grid and Sweep
#### Core: Generate Ring, Radial, and Sweep Geometry
#### Opengl: Render Geometry using GLEW

Usage in C# (Avalonia, [SFDM](https://pm.infoglobal.co.id:9443/sw-avi/sfd-demo) as example)

.csproj:
```
<PropertyGroup>
	<CopyLocalLockFileAssemblies>true</CopyLocalLockFileAssemblies>
</PropertyGroup>

<ItemGroup>
	<Content Include="path\libs\radar-1.0\bin\libradar_c_api.dll">
		<CopyToOutputDirectory>Always</CopyToOutputDirectory>
	</Content>
	<Content Include="path\libs\radar-1.0\bin\libradar_opengl.dll">
		<CopyToOutputDirectory>Always</CopyToOutputDirectory>
	</Content>
	<Content Include="path\libs\glew-2.1.0\bin\glew32.dll">
		<CopyToOutputDirectory>Always</CopyToOutputDirectory>
	</Content>
	<Content Include="path\libs\radar-1.0\bin\libradar_gl_api.dll">
		<CopyToOutputDirectory>Always</CopyToOutputDirectory>
	</Content>
	<Content Include="path\mingw64\bin\libgcc_s_seh-1.dll">
		<CopyToOutputDirectory>Always</CopyToOutputDirectory>
	</Content>
	<Content Include="path\mingw64\bin\libstdc++-6.dll">
		<CopyToOutputDirectory>Always</CopyToOutputDirectory>
	</Content>
	<Content Include="path\mingw64\bin\libwinpthread-1.dll">
		<CopyToOutputDirectory>Always</CopyToOutputDirectory>
	</Content>
</ItemGroup>
```


Desktop/Program.cs to call OpenGL (Avalonia Windows use SKIA by default):
```
    BuildAvaloniaApp()
        .With(new Win32PlatformOptions
        {
            RenderingMode = [Win32RenderingMode.Wgl],
        })
        .StartWithClassicDesktopLifetime(args);
```

Create a Wrapper class to call DllImport (InteropServices):
```
const string LIB = "libradar_gl_api";

[DllImport(LIB, CallingConvention = CallingConvention.Cdecl)]
public static extern int radar_gl_init();
```


Implement to UI using Avalonia.OpenGL, init and call inside `OnOpenGlRender`, as `OnOpenGlInit` possibly not have the context ready.
```
public class RadarGLView : OpenGlControlBase
{
  protected override void OnOpenGlRender(GlInterface gl, int fb)
  {
      if (!_glInitialized)
      {
          RadarGLWrapper.radar_gl_init();
          ctx = RadarGLWrapper.radar_create(Ring, Radial, 100, Speed, Detection);
          _glInitialized = true;
      }

      RadarGLWrapper.radar_render(ctx, (int)Bounds.Width, (int)Bounds.Height, DeltaTime);
  }
}
```