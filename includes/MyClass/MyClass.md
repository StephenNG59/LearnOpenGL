# Object3D class

## derived class
1.object3Dcube
2.object3Dcylinder
3.object3Dsphere

## about texture
```c++
	object3Dcube myObject();
	unsigned int idApple = myObject.AddTexture("My/Texture/Path/apple.png", ObjectTextureType::Ambient);
	unsigned int idBanana = myObject.AddTexture("My/Another/Texture/banana.jpg", ObjectTextureType::Diffuse);
	...

	myObject.ActivateTexture(idApple);		// default is active
	myObject.DisactiveTexture(idBanana);
	...

	myObject.Draw(camera, shader);

```

+ The corresponding uniform name in shader are 'materialEmission1'、'materialAmbient0'...

---

# Light class

