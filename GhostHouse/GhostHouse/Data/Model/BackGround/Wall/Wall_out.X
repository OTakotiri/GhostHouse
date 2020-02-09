xof 0303txt 0032
template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}

template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}


Material lambert1 {
 1.000000;1.000000;1.000000;1.000000;;
 0.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "kabe_soto.png";
 }
}

Material kabe_naka_kabe_naka_lambert2 {
 1.000000;1.000000;1.000000;1.000000;;
 0.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "kabe_tex.png";
 }
}

Frame kabe_sotoi {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Frame pCube4 {
  

  FrameTransformMatrix {
   1.000000,0.000000,0.000000,0.000000,0.000000,-0.000000,-1.000000,0.000000,0.000000,1.000000,-0.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
  }

  Mesh pCube4 {
   16;
   -12.500000;0.000000;0.000000;,
   12.500000;-0.000002;100.000000;,
   12.500000;0.000000;0.000000;,
   -12.500000;0.000000;100.000000;,
   -12.500000;0.000000;100.000000;,
   12.500000;-5.000000;100.000000;,
   12.500000;-0.000002;100.000000;,
   -12.500000;-5.000000;100.000000;,
   -12.500000;-5.000000;100.000000;,
   12.500000;-5.000000;0.000000;,
   12.500000;-5.000000;100.000000;,
   -12.500000;-5.000000;0.000000;,
   -12.500000;-5.000000;0.000000;,
   12.500000;-5.000000;-25.000000;,
   12.500000;-5.000000;0.000000;,
   -12.500000;-5.000000;-25.000000;;
   8;
   3;0,1,2;,
   3;1,0,3;,
   3;4,5,6;,
   3;5,4,7;,
   3;8,9,10;,
   3;9,8,11;,
   3;12,13,14;,
   3;13,12,15;;

   MeshNormals {
    16;
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;;
    8;
    3;0,1,2;,
    3;1,0,3;,
    3;4,5,6;,
    3;5,4,7;,
    3;8,9,10;,
    3;9,8,11;,
    3;12,13,14;,
    3;13,12,15;;
   }

   MeshMaterialList {
    2;
    8;
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1;
    { lambert1 }
    { kabe_naka_kabe_naka_lambert2 }
   }

   MeshTextureCoords c1 {
    16;
    0.344819;0.992702;,
    0.655181;0.007298;,
    0.655181;0.992702;,
    0.344819;0.007298;,
    0.969077;0.022299;,
    0.565890;0.040454;,
    0.565890;0.022299;,
    0.969077;0.040454;,
    0.456109;0.001976;,
    0.002227;0.998177;,
    0.002227;0.001975;,
    0.456109;0.998177;,
    0.280481;0.992731;,
    0.276636;0.996577;,
    0.276636;0.992731;,
    0.280481;0.996577;;
   }
  }
 }
}