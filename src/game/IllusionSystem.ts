import * as THREE from 'three';
import { LevelNode } from './types';

export class IllusionSystem {
  private camera: THREE.OrthographicCamera;
  private threshold: number = 0.05; // Screen space distance threshold

  constructor(camera: THREE.OrthographicCamera) {
    this.camera = camera;
  }

  /**
   * Projects a 3D point to normalized device coordinates (NDC) [-1, 1].
   */
  private project(point: THREE.Vector3): THREE.Vector2 {
    const projected = point.clone().project(this.camera);
    return new THREE.Vector2(projected.x, projected.y);
  }

  /**
   * Finds pairs of nodes that align in screen space.
   */
  public getIllusionConnections(nodes: LevelNode[]): [string, string][] {
    const connections: [string, string][] = [];
    const projectedNodes = nodes.map(node => ({
      id: node.id,
      screenPos: this.project(node.position)
    }));

    for (let i = 0; i < projectedNodes.length; i++) {
      for (let j = i + 1; j < projectedNodes.length; j++) {
        const dist = projectedNodes[i].screenPos.distanceTo(projectedNodes[j].screenPos);
        if (dist < this.threshold) {
          connections.push([projectedNodes[i].id, projectedNodes[j].id]);
        }
      }
    }

    return connections;
  }
}
