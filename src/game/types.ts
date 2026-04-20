import * as THREE from 'three';

export type Vector3Tuple = [number, number, number];

export interface LevelNode {
  id: string;
  position: THREE.Vector3;
  connections: string[]; // IDs of other nodes connected in 3D
}

export interface LevelData {
  id: string;
  name: string;
  blocks: BlockData[];
  startNodeId: string;
  goalNodeId: string;
}

export interface BlockData {
  type: 'cube' | 'stair' | 'bridge' | 'pillar' | 'rotating';
  position: Vector3Tuple;
  rotation?: Vector3Tuple;
  scale?: Vector3Tuple;
  color?: string;
  nodes: {
    id: string;
    offset: Vector3Tuple;
    connections: string[]; // Relative IDs or absolute if needed
  }[];
}

export interface GameState {
  currentLevelIndex: number;
  isMoving: boolean;
  cameraRotation: number; // 0, 1, 2, 3 (multiples of 90 deg)
}

// OpenGL-related types
export interface GLShaderConfig {
  shaderName: string;
  uniforms?: Record<string, any>;
}

export interface GLLightConfig {
  type: 'directional' | 'point' | 'spot';
  position: THREE.Vector3;
  color: THREE.Color | number;
  intensity: number;
  castShadow?: boolean;
  shadowMapSize?: number;
}

export interface GLRenderingMode {
  mode: 'standard' | 'phong' | 'toon' | 'normal-map' | 'parallax';
  enabled: boolean;
}
