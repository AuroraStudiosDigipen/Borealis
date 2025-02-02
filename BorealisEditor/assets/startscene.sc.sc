Scene: startscene.sc
Entities:
  - EntityID: 9578477383331735772
    Tag Component:
      IsActive: true
      Tag: Mgr
      Layer: 1
    Transform Component:
      Translate: [0, 0, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
      ParentID: 0
      ChildrenID:
        {}
    ScriptComponent:
      MainMenuManager:
        Arrow1:
          Type: GameObject
          Data: 14932715688160371535
        credits:
          Type: GameObject
          Data: 9077798465780225788
        startGame:
          Type: GameObject
          Data: 8497928453328679653
        settings:
          Type: GameObject
          Data: 468161840520527781
        Arrow2:
          Type: GameObject
          Data: 12542243017936634495
        quitGame:
          Type: GameObject
          Data: 17373744394334001911
  - EntityID: 14932715688160371535
    Tag Component:
      IsActive: true
      Tag: Arrow1
      Layer: 1
    Transform Component:
      Translate: [3.57254934, 26.7786331, -6.08676928e-05]
      Rotation: [0, -0, -179.999893]
      Scale: [1.5, 1.5, 1.5]
      ParentID: 0
      ChildrenID:
        {}
    Sprite Renderer Component:
      Colour: [1, 1, 1, 1]
      Texture: 10125085886872468809
      Tiling Factor: 1
  - EntityID: 12542243017936634495
    Tag Component:
      IsActive: true
      Tag: Arrow2
      Layer: 1
    Transform Component:
      Translate: [26.6266403, 25.9481335, -6.08676928e-05]
      Rotation: [0, -0, 0]
      Scale: [1.5, 1.5, 1.5]
      ParentID: 0
      ChildrenID:
        {}
    Sprite Renderer Component:
      Colour: [1, 1, 1, 1]
      Texture: 10125085886872468809
      Tiling Factor: 1
  - EntityID: 17373744394334001911
    Tag Component:
      IsActive: true
      Tag: QuitText
      Layer: 1
    Transform Component:
      Translate: [14.7227755, -14.6283417, -0.494726479]
      Rotation: [0, -0, 0]
      Scale: [0.439999998, 0.439999998, 0.999976456]
      ParentID: 0
      ChildrenID:
        {}
    Text Component:
      Text: QUIT GAME
      Font:
        {}
      Font Size: 2
      Colour: [0.698039234, 0.733333349, 0.945098042, 1]
  - EntityID: 9077798465780225788
    Tag Component:
      IsActive: true
      Tag: CreditText
      Layer: 1
    Transform Component:
      Translate: [15.2900448, -11.5936689, 0]
      Rotation: [0, -0, 0]
      Scale: [0.439999998, 0.439999998, 0.999969482]
      ParentID: 0
      ChildrenID:
        {}
    Text Component:
      Text: CREDITS
      Font:
        {}
      Font Size: 2
      Colour: [0.698039234, 0.733333349, 0.945098042, 1]
  - EntityID: 8497928453328679653
    Tag Component:
      IsActive: true
      Tag: StartGameText
      Layer: 1
    Transform Component:
      Translate: [13.872776, -6.5, 0]
      Rotation: [0, -0, 0]
      Scale: [0.439999998, 0.439999998, 0.99999994]
      ParentID: 0
      ChildrenID:
        {}
    Text Component:
      Text: START GAME
      Font:
        {}
      Font Size: 2
      Colour: [0.698039234, 0.733333349, 0.945098042, 1]
  - EntityID: 468161840520527781
    Tag Component:
      IsActive: true
      Tag: SettingsText
      Layer: 1
    Transform Component:
      Translate: [14.8200474, -8.97000027, 0]
      Rotation: [0, -0, 0]
      Scale: [0.439999998, 0.439999998, 0.999976397]
      ParentID: 0
      ChildrenID:
        {}
    Text Component:
      Text: SETTINGS
      Font:
        {}
      Font Size: 2
      Colour: [0.698039234, 0.733333349, 0.945098042, 1]
  - EntityID: 14883438960780730428
    Tag Component:
      IsActive: true
      Tag: PointLight
      Layer: 1
    Transform Component:
      Translate: [-56.1768494, 84.2947998, 116.368301]
      Rotation: [-169.098938, -87.6595688, -179.9991]
      Scale: [10.3519945, 10.3518887, 28.9855347]
      ParentID: 16621025665307153688
      ChildrenID:
        {}
    Light Component:
      Type: Point
      Range: 15
      Spot Angle: 30
      Color: [1, 0.947970212, 0.766338944, 1]
      Intensity: 0.699999988
      CastShadow: true
  - EntityID: 18213790858618815895
    Tag Component:
      IsActive: true
      Tag: PointLight
      Layer: 1
    Transform Component:
      Translate: [12.0675068, 105.807419, 119.772522]
      Rotation: [-169.098938, -87.6595688, -179.9991]
      Scale: [10.3519945, 10.3518887, 28.9855347]
      ParentID: 16621025665307153688
      ChildrenID:
        {}
    Light Component:
      Type: Point
      Range: 15
      Spot Angle: 30
      Color: [0.304326147, 0.787955999, 0.847310603, 1]
      Intensity: 0.400000006
      CastShadow: true
  - EntityID: 16013459016095721907
    Tag Component:
      IsActive: true
      Tag: PointLight
      Layer: 1
    Transform Component:
      Translate: [16.6315937, 62.6062393, 116.76091]
      Rotation: [-169.099503, -87.659668, -179.9991]
      Scale: [10.3519878, 10.3519001, 28.9855309]
      ParentID: 16621025665307153688
      ChildrenID:
        {}
    Light Component:
      Type: Point
      Range: 15
      Spot Angle: 30
      Color: [1, 0.947970212, 0.766338944, 1]
      Intensity: 0.400000006
      CastShadow: true
  - EntityID: 5348759906943006856
    Tag Component:
      IsActive: true
      Tag: LoadingScreen
      Layer: 1
    Transform Component:
      Translate: [0, 7.27595761e-12, 0]
      Rotation: [0, -0, 0]
      Scale: [0.99999994, 0.99999994, 0.99999994]
      ParentID: 11927724426653065229
      ChildrenID:
        {}
    Sprite Renderer Component:
      Colour: [1, 1, 1, 0]
      Texture: 9062122544801465634
      Tiling Factor: 1
    Canvas Renderer Component:
      Allow Passthrough: false
  - EntityID: 7081433347072769186
    Tag Component:
      IsActive: true
      Tag: WallLeft
      Layer: 1
    Transform Component:
      Translate: [-110.204628, -3.77080655, -8.68869114]
      Rotation: [0, 89.9801941, 0]
      Scale: [0.295602709, 0.176111519, 0.0788031071]
      ParentID: 0
      ChildrenID:
        {}
    Mesh Filter Component:
      Model: 2787534781984028591
    Mesh Renderer Component:
      Material: 9966320878143753008
      Cast Shadow: true
      Enabled: true
  - EntityID: 3381513916313953992
    Tag Component:
      IsActive: true
      Tag: BlackScreen
      Layer: 1
    Transform Component:
      Translate: [0, 0.00443332875, 0]
      Rotation: [0, -0, 0]
      Scale: [0.999992549, 0.999991417, 0.99999541]
      ParentID: 11927724426653065229
      ChildrenID:
        {}
    Sprite Renderer Component:
      Colour: [0, 0, 0, 0]
      Tiling Factor: 1
    Canvas Renderer Component:
      Allow Passthrough: false
  - EntityID: 11927724426653065229
    Tag Component:
      IsActive: true
      Tag: Canvas
      Layer: 1
    Transform Component:
      Translate: [0, -0.00107479095, 0]
      Rotation: [0, -0, 0]
      Scale: [25.9499989, 15.9399996, 1]
      ParentID: 0
      ChildrenID:
        UUID: 16619051026072466634
        UUID: 5348759906943006856
        UUID: 3381513916313953992
    Canvas Component:
      Render Mode: Screen Space
  - EntityID: 8042165576996827541
    Tag Component:
      IsActive: true
      Tag: Ground
      Layer: 1
    Transform Component:
      Translate: [2.44784594, -16.0987377, -11.1235056]
      Rotation: [0, 89.9801941, 0]
      Scale: [0.263197422, 0.0570731908, 0.297234744]
      ParentID: 0
      ChildrenID:
        {}
    Mesh Filter Component:
      Model: 4064270159941428259
    Mesh Renderer Component:
      Material: 2680068658231387163
      Cast Shadow: true
      Enabled: true
  - EntityID: 12176194088803460940
    Tag Component:
      IsActive: true
      Tag: BGM
      Layer: 1
    Transform Component:
      Translate: [0, 0, 350]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
      ParentID: 0
      ChildrenID:
        {}
    Audio Source Component:
      Is Looping: false
      Is Mute: false
      Is Playing: false
      Volume: 1
    ScriptComponent:
      BGMManagerWithTransitions:
        bgmSource:
          Type: AudioSource
          Data: 12176194088803460940
        volume:
          Type: Float
          Data: 30
        sadBGM:
          Type: AudioClip
          Data: 6221567316633784286
  - EntityID: 15064286722699785201
    Tag Component:
      IsActive: true
      Tag: BtmMid
      Layer: 1
    Transform Component:
      Translate: [-0.144081786, -3.77092791, 84.0982361]
      Rotation: [0, 89.9801941, 0]
      Scale: [0.0917962119, 0.223971501, 0.730203271]
      ParentID: 0
      ChildrenID:
        {}
    Mesh Filter Component:
      Model: 6979874975098773996
    Mesh Renderer Component:
      Material: 9966320878143753008
      Cast Shadow: true
      Enabled: true
  - EntityID: 16621025665307153688
    Tag Component:
      IsActive: true
      Tag: TV
      Layer: 1
    Transform Component:
      Translate: [-18.4940605, -14.8599377, -13.2693787]
      Rotation: [0, 46.5423393, 0]
      Scale: [0.0965999663, 0.0966000035, 0.0965999663]
      ParentID: 0
      ChildrenID:
        UUID: 14883438960780730428
        UUID: 7521457112367002628
        UUID: 16013459016095721907
        UUID: 18213790858618815895
    Mesh Filter Component:
      Model: 17037002090944461966
    Mesh Renderer Component:
      Material: 1789256408709549607
      Cast Shadow: true
      Enabled: true
  - EntityID: 6111607614294162027
    Tag Component:
      IsActive: true
      Tag: Camera
      Layer: 1
    Camera Component:
      Primary Camera: true
      Fixed Aspect Ratio: false
      Camera:
        Camera Type: Perspective
        Perspective Near Clip: 0.00999999978
        Perspective Far Clip: 1000
        Perspective FOV: 70
        Orthographic Near Clip: -1
        Orthographic Far Clip: 1000
        Orthographic Size: 350
    Transform Component:
      Translate: [2.49231744, -7.13060284, 21.2115383]
      Rotation: [0, -0, 0]
      Scale: [0.999999762, 1, 0.999999762]
      ParentID: 0
      ChildrenID:
        {}
  - EntityID: 5031451480922568663
    Tag Component:
      IsActive: true
      Tag: TopMid
      Layer: 1
    Transform Component:
      Translate: [-0.10491544, -3.77092791, -109.834755]
      Rotation: [0, 89.9801941, 0]
      Scale: [0.0917970836, 0.236313611, 0.761720955]
      ParentID: 0
      ChildrenID:
        {}
    Mesh Filter Component:
      Model: 15687927543376717783
    Mesh Renderer Component:
      Material: 9966320878143753008
      Cast Shadow: true
      Enabled: true
  - EntityID: 7521457112367002628
    Tag Component:
      IsActive: true
      Tag: PointLight
      Layer: 1
    Transform Component:
      Translate: [-67.6777573, 58.01577, 116.513557]
      Rotation: [-169.098938, -87.6595688, -179.9991]
      Scale: [10.3519945, 10.3518887, 28.9855347]
      ParentID: 16621025665307153688
      ChildrenID:
        {}
    Light Component:
      Type: Point
      Range: 15
      Spot Angle: 30
      Color: [0.425828069, 0.394729674, 0.979178727, 1]
      Intensity: 0.5
      CastShadow: true
  - EntityID: 9777881039264254949
    Tag Component:
      IsActive: true
      Tag: Roof
      Layer: 1
    Transform Component:
      Translate: [127.201126, -3.77092791, -8.65805912]
      Rotation: [0, 89.9801941, 0]
      Scale: [0.238641322, 0.271110475, 0.0788031071]
      ParentID: 0
      ChildrenID:
        {}
    Mesh Filter Component:
      Model: 13426986549347235599
    Mesh Renderer Component:
      Material: 9966320878143753008
      Cast Shadow: true
      Enabled: true
  - EntityID: 10350428871025083594
    Tag Component:
      IsActive: true
      Tag: WallRight
      Layer: 1
    Transform Component:
      Translate: [0.458374202, 38.7449036, -8.7019043]
      Rotation: [89.9606094, 89.9801254, 0.00984802004]
      Scale: [0.26335007, 0.539146781, 0.0788030773]
      ParentID: 0
      ChildrenID:
        {}
    Mesh Filter Component:
      Model: 13426986549347235599
    Mesh Renderer Component:
      Material: 9966320878143753008
      Cast Shadow: true
      Enabled: true
  - EntityID: 13693108153651462649
    Tag Component:
      IsActive: true
      Tag: Light
      Layer: 1
    Transform Component:
      Translate: [0, 0, 0]
      Rotation: [10.8999996, -60.2000008, 0]
      Scale: [1, 1, 2.79999995]
      ParentID: 0
      ChildrenID:
        {}
    Light Component:
      Type: Directional
      Range: 10
      Spot Angle: 30
      Color: [0.923655272, 0.923655272, 0.923655272, 1]
      Intensity: 0
      CastShadow: true