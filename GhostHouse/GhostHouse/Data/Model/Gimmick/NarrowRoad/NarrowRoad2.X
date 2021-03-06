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


Material lambert2 {
 1.000000;1.000000;1.000000;1.000000;;
 0.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;

 TextureFilename {
  "narrow road_tex.png";
 }
}

Frame narrowroad2 {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Frame pCube6 {
  

  FrameTransformMatrix {
   1.000000,0.000000,0.000000,0.000000,0.000000,-0.000000,-1.000000,0.000000,0.000000,1.000000,-0.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
  }

  Mesh pCube6 {
   23;
   -12.500000;-3.303954;0.000000;,
   -3.303955;3.303954;0.000000;,
   -12.500000;3.303954;0.000000;,
   -0.000000;0.000000;0.000000;,
   3.303955;-3.303954;0.000000;,
   -12.500000;3.303954;-25.000000;,
   -3.303955;3.303954;0.000000;,
   -3.303955;3.303954;-25.000000;,
   -12.500000;3.303954;0.000000;,
   3.303955;-3.303954;-25.000000;,
   -12.500000;-3.303954;0.000000;,
   -12.500000;-3.303954;-25.000000;,
   3.303955;-3.303954;0.000000;,
   -3.303955;3.303954;-25.000000;,
   -3.303955;12.500000;0.000000;,
   -3.303955;12.500000;-25.000000;,
   -3.303955;3.303954;0.000000;,
   3.303955;12.500000;0.000000;,
   -3.303955;12.500000;0.000000;,
   3.303955;12.500000;-25.000000;,
   3.303955;-3.303954;0.000000;,
   3.303955;-3.303954;-25.000000;,
   3.303955;12.500000;0.000000;;
   14;
   3;0,1,2;,
   3;0,3,1;,
   3;0,4,3;,
   3;5,6,7;,
   3;6,5,8;,
   3;9,10,11;,
   3;10,9,12;,
   3;13,14,15;,
   3;14,13,16;,
   3;17,3,4;,
   3;17,1,3;,
   3;17,18,1;,
   3;19,20,21;,
   3;20,19,22;;

   MeshNormals {
    23;
    -0.000000;-0.000000;1.000000;,
    -0.000000;-0.000000;1.000000;,
    -0.000000;-0.000000;1.000000;,
    -0.000000;-0.000000;1.000000;,
    -0.000000;-0.000000;1.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;0.000000;1.000000;,
    1.000000;0.000000;0.000000;,
    1.000000;0.000000;0.000000;,
    1.000000;0.000000;0.000000;,
    1.000000;0.000000;0.000000;;
    14;
    3;0,1,2;,
    3;0,3,1;,
    3;0,4,3;,
    3;5,6,7;,
    3;6,5,8;,
    3;9,10,11;,
    3;10,9,12;,
    3;13,14,15;,
    3;14,13,16;,
    3;17,3,4;,
    3;17,1,3;,
    3;17,18,1;,
    3;19,20,21;,
    3;20,19,22;;
   }

   MeshMaterialList {
    1;
    14;
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0;
    { lambert2 }
   }

   MeshTextureCoords c1 {
    23;
    0.110328;0.016220;,
    0.370031;0.202530;,
    0.110471;0.202729;,
    0.463215;0.109203;,
    0.556399;0.015878;,
    0.093963;0.976170;,
    0.544154;0.479752;,
    0.544154;0.976170;,
    0.093963;0.479752;,
    0.544154;0.976170;,
    0.093963;0.479752;,
    0.093963;0.976170;,
    0.544154;0.479752;,
    0.544154;0.976170;,
    0.093963;0.479752;,
    0.093963;0.976170;,
    0.544154;0.479752;,
    0.556741;0.461949;,
    0.370230;0.462092;,
    0.093963;0.976170;,
    0.544154;0.479752;,
    0.544154;0.976170;,
    0.093963;0.479752;;
   }
  }
 }
}