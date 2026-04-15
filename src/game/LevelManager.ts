import { LevelData } from './types';

export const LEVELS: LevelData[] = [
  {
    id: 'level-1',
    name: 'The Beginning',
    startNodeId: 'b1-n1',
    goalNodeId: 'b3-n1',
    blocks: [
      {
        type: 'cube',
        position: [0, 0, 0],
        color: '#a8d8ea',
        nodes: [{ id: 'b1-n1', offset: [0, 0.5, 0], connections: ['b2-n1'] }]
      },
      {
        type: 'cube',
        position: [1, 0, 0],
        color: '#a8d8ea',
        nodes: [{ id: 'b2-n1', offset: [0, 0.5, 0], connections: ['b1-n1', 'b3-n1'] }]
      },
      {
        type: 'cube',
        position: [2, 0, 0],
        color: '#ffccf9',
        nodes: [{ id: 'b3-n1', offset: [0, 0.5, 0], connections: ['b2-n1'] }]
      }
    ]
  },
  {
    id: 'level-2',
    name: 'The Illusion',
    startNodeId: 'start',
    goalNodeId: 'goal',
    blocks: [
      // Lower platform
      {
        type: 'cube',
        position: [0, 0, 0],
        color: '#a8d8ea',
        nodes: [{ id: 'start', offset: [0, 0.5, 0], connections: ['p1'] }]
      },
      {
        type: 'cube',
        position: [1, 0, 0],
        color: '#a8d8ea',
        nodes: [{ id: 'p1', offset: [0, 0.5, 0], connections: ['start'] }]
      },
      // Upper platform (disconnected in 3D)
      {
        type: 'cube',
        position: [1, 2, -2],
        color: '#ffccf9',
        nodes: [{ id: 'p2', offset: [0, 0.5, 0], connections: ['goal'] }]
      },
      {
        type: 'cube',
        position: [2, 2, -2],
        color: '#ffccf9',
        nodes: [{ id: 'goal', offset: [0, 0.5, 0], connections: ['p2'] }]
      }
    ]
  },
  {
    id: 'level-3',
    name: 'The Spiral',
    startNodeId: 's1',
    goalNodeId: 'g1',
    blocks: [
      { type: 'cube', position: [0, 0, 0], nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['s2'] }] },
      { type: 'cube', position: [1, 0, 0], nodes: [{ id: 's2', offset: [0, 0.5, 0], connections: ['s1', 's3'] }] },
      { type: 'cube', position: [1, 0, 1], nodes: [{ id: 's3', offset: [0, 0.5, 0], connections: ['s2', 's4'] }] },
      { type: 'cube', position: [0, 0, 1], nodes: [{ id: 's4', offset: [0, 0.5, 0], connections: ['s3', 's5'] }] },
      { type: 'cube', position: [0, 1, 1], nodes: [{ id: 's5', offset: [0, 0.5, 0], connections: ['s4', 's6'] }] },
      { type: 'cube', position: [0, 2, 1], nodes: [{ id: 's6', offset: [0, 0.5, 0], connections: ['s5', 'g1'] }] },
      { type: 'cube', position: [0, 2, 0], nodes: [{ id: 'g1', offset: [0, 0.5, 0], connections: ['s6'] }] },
    ]
  },
  {
    id: 'level-4',
    name: 'The Bridge of Faith',
    startNodeId: 's1',
    goalNodeId: 'g1',
    blocks: [
      { type: 'cube', position: [0, 0, 0], color: '#a8d8ea', nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['p1'] }] },
      { type: 'cube', position: [1, 0, 0], color: '#a8d8ea', nodes: [{ id: 'p1', offset: [0, 0.5, 0], connections: ['s1', 'p2'] }] },
      { type: 'cube', position: [2, 0, 0], color: '#a8d8ea', nodes: [{ id: 'p2', offset: [0, 0.5, 0], connections: ['p1'] }] },
      
      // Far away platform
      { type: 'cube', position: [2, 4, -4], color: '#ffccf9', nodes: [{ id: 'p3', offset: [0, 0.5, 0], connections: ['p4'] }] },
      { type: 'cube', position: [3, 4, -4], color: '#ffccf9', nodes: [{ id: 'p4', offset: [0, 0.5, 0], connections: ['p3', 'g1'] }] },
      { type: 'cube', position: [4, 4, -4], color: '#ffccf9', nodes: [{ id: 'g1', offset: [0, 0.5, 0], connections: ['p4'] }] },
      
      // Floating pillars for visual cues
      { type: 'pillar', position: [2, 1, 0], color: '#f0f0f0', nodes: [] },
      { type: 'pillar', position: [2, 2, 0], color: '#f0f0f0', nodes: [] },
      { type: 'pillar', position: [2, 3, 0], color: '#f0f0f0', nodes: [] },
    ]
  },
  {
    id: 'level-5',
    name: 'The Grand Palace',
    startNodeId: 's1',
    goalNodeId: 'goal',
    blocks: [
      // Central Plaza
      { type: 'cube', position: [0, 0, 0], color: '#7bed9f', nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['p1', 'p2'] }] },
      { type: 'cube', position: [1, 0, 0], color: '#7bed9f', nodes: [{ id: 'p1', offset: [0, 0.5, 0], connections: ['s1', 'p3'] }] },
      { type: 'cube', position: [0, 0, 1], color: '#7bed9f', nodes: [{ id: 'p2', offset: [0, 0.5, 0], connections: ['s1', 'p4'] }] },
      { type: 'cube', position: [1, 0, 1], color: '#7bed9f', nodes: [{ id: 'p3', offset: [0, 0.5, 0], connections: ['p1', 'p4'] }] },
      { type: 'cube', position: [0, 0, 2], color: '#7bed9f', nodes: [{ id: 'p4', offset: [0, 0.5, 0], connections: ['p2', 'p3', 'stairs1'] }] },
      
      // Stairs up
      { type: 'cube', position: [0, 1, 3], color: '#eccc68', nodes: [{ id: 'stairs1', offset: [0, 0.5, 0], connections: ['p4', 'stairs2'] }] },
      { type: 'cube', position: [0, 2, 4], color: '#eccc68', nodes: [{ id: 'stairs2', offset: [0, 0.5, 0], connections: ['stairs1', 'upper1'] }] },
      
      // Upper Balcony
      { type: 'cube', position: [0, 3, 5], color: '#70a1ff', nodes: [{ id: 'upper1', offset: [0, 0.5, 0], connections: ['stairs2', 'upper2'] }] },
      { type: 'cube', position: [1, 3, 5], color: '#70a1ff', nodes: [{ id: 'upper2', offset: [0, 0.5, 0], connections: ['upper1', 'upper3'] }] },
      { type: 'cube', position: [2, 3, 5], color: '#70a1ff', nodes: [{ id: 'upper3', offset: [0, 0.5, 0], connections: ['upper2', 'upper4'] }] },
      { type: 'cube', position: [3, 3, 5], color: '#70a1ff', nodes: [{ id: 'upper4', offset: [0, 0.5, 0], connections: ['upper3', 'bridge1'] }] },
      
      // The Illusion Bridge
      { type: 'cube', position: [5, 3, 5], color: '#ff7f50', nodes: [{ id: 'bridge1', offset: [0, 0.5, 0], connections: ['upper4', 'bridge2'] }] },
      { type: 'cube', position: [5, 3, 4], color: '#ff7f50', nodes: [{ id: 'bridge2', offset: [0, 0.5, 0], connections: ['bridge1', 'bridge3'] }] },
      { type: 'cube', position: [5, 3, 3], color: '#ff7f50', nodes: [{ id: 'bridge3', offset: [0, 0.5, 0], connections: ['bridge2'] }] },
      
      // Disconnected Goal Island
      { type: 'cube', position: [5, 6, 0], color: '#ffa502', nodes: [{ id: 'goal_p1', offset: [0, 0.5, 0], connections: ['goal'] }] },
      { type: 'cube', position: [6, 6, 0], color: '#ffd32a', nodes: [{ id: 'goal', offset: [0, 0.5, 0], connections: ['goal_p1'] }] },
      
      // Decorative Pillars
      { type: 'pillar', position: [5, 0, 0], color: '#dfe4ea', nodes: [] },
      { type: 'pillar', position: [5, 1, 0], color: '#dfe4ea', nodes: [] },
      { type: 'pillar', position: [5, 2, 0], color: '#dfe4ea', nodes: [] },
      { type: 'pillar', position: [5, 3, 0], color: '#dfe4ea', nodes: [] },
      { type: 'pillar', position: [5, 4, 0], color: '#dfe4ea', nodes: [] },
      { type: 'pillar', position: [5, 5, 0], color: '#dfe4ea', nodes: [] },
      
      // Extra decorative blocks for plants
      { type: 'cube', position: [2, 0, 0], color: '#7bed9f', nodes: [] },
      { type: 'cube', position: [-1, 0, 1], color: '#7bed9f', nodes: [] },
      { type: 'cube', position: [3, 3, 4], color: '#70a1ff', nodes: [] },
    ]
  },
  {
    id: 'level-6',
    name: 'The Clockwork',
    startNodeId: 's1',
    goalNodeId: 'goal',
    blocks: [
      // Start Platform
      { type: 'cube', position: [0, 0, 0], color: '#a8d8ea', nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['s2'] }] },
      { type: 'cube', position: [1, 0, 0], color: '#a8d8ea', nodes: [{ id: 's2', offset: [0, 0.5, 0], connections: ['s1', 'r_arm1', 'r_arm2'] }] },
      
      // Rotating Block (Center)
      { 
        type: 'rotating', 
        position: [3, 0, 0], 
        color: '#ff9ff3', 
        nodes: [
          { id: 'r_center', offset: [0, 0.5, 0], connections: ['r_arm1', 'r_arm2'] },
          { id: 'r_arm1', offset: [-1, 0.5, 0], connections: ['r_center', 's2', 'g1'] },
          { id: 'r_arm2', offset: [1, 0.5, 0], connections: ['r_center', 's2', 'g1'] }
        ] 
      },
      
      // Goal Platform
      { type: 'cube', position: [3, 0, 2], color: '#feca57', nodes: [{ id: 'g1', offset: [0, 0.5, 0], connections: ['goal', 'r_arm1', 'r_arm2'] }] },
      { type: 'cube', position: [3, 0, 3], color: '#feca57', nodes: [{ id: 'goal', offset: [0, 0.5, 0], connections: ['g1'] }] },
      
      // Secondary path (requires illusion + rotation)
      { type: 'cube', position: [3, 2, -3], color: '#ff6b6b', nodes: [{ id: 'p1', offset: [0, 0.5, 0], connections: ['p2'] }] },
      { type: 'cube', position: [3, 2, -2], color: '#ff6b6b', nodes: [{ id: 'p2', offset: [0, 0.5, 0], connections: ['p1'] }] },
      
      // Decorative
      { type: 'pillar', position: [3, -1, 0], color: '#dfe4ea', nodes: [] },
      { type: 'pillar', position: [3, -2, 0], color: '#dfe4ea', nodes: [] },
    ]
  },
  {
    id: 'level-7',
    name: 'The Labyrinth',
    startNodeId: 's1',
    goalNodeId: 'goal',
    blocks: [
      // Start Area
      { type: 'cube', position: [0, 0, 0], color: '#55efc4', nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['s2'] }] },
      { type: 'cube', position: [1, 0, 0], color: '#55efc4', nodes: [{ id: 's2', offset: [0, 0.5, 0], connections: ['s1', 'r1_arm1'] }] },
      
      // First Rotating Block
      { 
        type: 'rotating', 
        position: [3, 0, 0], 
        color: '#81ecec', 
        nodes: [
          { id: 'r1_center', offset: [0, 0.5, 0], connections: ['r1_arm1', 'r1_arm2'] },
          { id: 'r1_arm1', offset: [-1, 0.5, 0], connections: ['r1_center', 's2', 'm1'] },
          { id: 'r1_arm2', offset: [1, 0.5, 0], connections: ['r1_center', 's2', 'm1'] }
        ] 
      },

      // Middle Island
      { type: 'cube', position: [3, 0, -2], color: '#74b9ff', nodes: [{ id: 'm1', offset: [0, 0.5, 0], connections: ['r1_arm1', 'r1_arm2', 'm2'] }] },
      { type: 'cube', position: [3, 0, -3], color: '#74b9ff', nodes: [{ id: 'm2', offset: [0, 0.5, 0], connections: ['m1', 'r2_arm1'] }] },

      // Second Rotating Block (Vertical Illusion)
      { 
        type: 'rotating', 
        position: [3, 2, -5], 
        color: '#a29bfe', 
        nodes: [
          { id: 'r2_center', offset: [0, 0.5, 0], connections: ['r2_arm1', 'r2_arm2'] },
          { id: 'r2_arm1', offset: [0, 0.5, 1], connections: ['r2_center', 'm2'] },
          { id: 'r2_arm2', offset: [0, 0.5, -1], connections: ['r2_center', 'g1'] }
        ] 
      },

      // Goal Area
      { type: 'cube', position: [3, 2, -7], color: '#ffeaa7', nodes: [{ id: 'g1', offset: [0, 0.5, 0], connections: ['goal', 'r2_arm2'] }] },
      { type: 'cube', position: [3, 2, -8], color: '#ffeaa7', nodes: [{ id: 'goal', offset: [0, 0.5, 0], connections: ['g1'] }] },

      // Distraction / Illusion path
      { type: 'cube', position: [5, 0, 0], color: '#ff7675', nodes: [{ id: 'd1', offset: [0, 0.5, 0], connections: ['d2'] }] },
      { type: 'cube', position: [6, 0, 0], color: '#ff7675', nodes: [{ id: 'd2', offset: [0, 0.5, 0], connections: ['d1'] }] },
    ]
  },
  {
    id: 'level-8',
    name: 'Ascension',
    startNodeId: 's1',
    goalNodeId: 'goal',
    blocks: [
      // Base
      { type: 'cube', position: [0, 0, 0], color: '#fab1a0', nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['s2'] }] },
      { type: 'cube', position: [1, 0, 0], color: '#fab1a0', nodes: [{ id: 's2', offset: [0, 0.5, 0], connections: ['s1', 'stair1'] }] },
      
      // Stairs up
      { type: 'cube', position: [2, 1, 0], color: '#ff7675', nodes: [{ id: 'stair1', offset: [0, 0.5, 0], connections: ['s2', 'stair2'] }] },
      { type: 'cube', position: [3, 2, 0], color: '#ff7675', nodes: [{ id: 'stair2', offset: [0, 0.5, 0], connections: ['stair1', 'plat1'] }] },

      // Platform 1
      { type: 'cube', position: [4, 2, 0], color: '#fd79a8', nodes: [{ id: 'plat1', offset: [0, 0.5, 0], connections: ['stair2', 'r1_arm1'] }] },

      // Rotating Bridge
      { 
        type: 'rotating', 
        position: [4, 2, 2], 
        color: '#e84393', 
        nodes: [
          { id: 'r1_center', offset: [0, 0.5, 0], connections: ['r1_arm1', 'r1_arm2'] },
          { id: 'r1_arm1', offset: [0, 0.5, -1], connections: ['r1_center', 'plat1'] },
          { id: 'r1_arm2', offset: [0, 0.5, 1], connections: ['r1_center', 'plat2'] }
        ] 
      },

      // Platform 2
      { type: 'cube', position: [4, 2, 4], color: '#fd79a8', nodes: [{ id: 'plat2', offset: [0, 0.5, 0], connections: ['r1_arm2', 'stair3'] }] },

      // Stairs up again (Illusion required)
      { type: 'cube', position: [2, 4, 4], color: '#d63031', nodes: [{ id: 'stair3', offset: [0, 0.5, 0], connections: ['plat2', 'stair4'] }] },
      { type: 'cube', position: [1, 5, 4], color: '#d63031', nodes: [{ id: 'stair4', offset: [0, 0.5, 0], connections: ['stair3', 'g1'] }] },

      // Goal
      { type: 'cube', position: [0, 5, 4], color: '#fdcb6e', nodes: [{ id: 'g1', offset: [0, 0.5, 0], connections: ['stair4', 'goal'] }] },
      { type: 'cube', position: [-1, 5, 4], color: '#fdcb6e', nodes: [{ id: 'goal', offset: [0, 0.5, 0], connections: ['g1'] }] },

      // Pillars for aesthetics
      { type: 'pillar', position: [4, 0, 0], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [4, 1, 0], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [4, 0, 4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [4, 1, 4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [0, 0, 4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [0, 1, 4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [0, 2, 4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [0, 3, 4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [0, 4, 4], color: '#dfe6e9', nodes: [] },
    ]
  },
  {
    id: 'level-9',
    name: 'The Impossible Machine',
    startNodeId: 's1',
    goalNodeId: 'goal',
    blocks: [
      // Starting Area
      { type: 'cube', position: [0, 0, 0], color: '#00cec9', nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['s2'] }] },
      { type: 'cube', position: [1, 0, 0], color: '#00cec9', nodes: [{ id: 's2', offset: [0, 0.5, 0], connections: ['s1', 'r1_arm1'] }] },
      
      // Rotating Block 1
      { 
        type: 'rotating', 
        position: [3, 0, 0], 
        color: '#fdcb6e', 
        nodes: [
          { id: 'r1_center', offset: [0, 0.5, 0], connections: ['r1_arm1', 'r1_arm2'] },
          { id: 'r1_arm1', offset: [-1, 0.5, 0], connections: ['r1_center', 's2'] },
          { id: 'r1_arm2', offset: [1, 0.5, 0], connections: ['r1_center', 'm1'] }
        ] 
      },

      // Mid Section (requires illusion to connect to r2)
      { type: 'cube', position: [5, 0, 0], color: '#ff7675', nodes: [{ id: 'm1', offset: [0, 0.5, 0], connections: ['r1_arm2', 'm2'] }] },
      { type: 'cube', position: [6, 0, 0], color: '#ff7675', nodes: [{ id: 'm2', offset: [0, 0.5, 0], connections: ['m1', 'r2_arm1'] }] },

      // Rotating Block 2 (Vertical Illusion)
      { 
        type: 'rotating', 
        position: [6, 2, -2], 
        color: '#6c5ce7', 
        nodes: [
          { id: 'r2_center', offset: [0, 0.5, 0], connections: ['r2_arm1', 'r2_arm2'] },
          { id: 'r2_arm1', offset: [0, 0.5, 1], connections: ['r2_center', 'm2'] },
          { id: 'r2_arm2', offset: [0, 0.5, -1], connections: ['r2_center', 'plat1'] }
        ] 
      },

      // High Platform
      { type: 'cube', position: [6, 2, -4], color: '#00b894', nodes: [{ id: 'plat1', offset: [0, 0.5, 0], connections: ['r2_arm2', 'plat2'] }] },
      { type: 'cube', position: [5, 2, -4], color: '#00b894', nodes: [{ id: 'plat2', offset: [0, 0.5, 0], connections: ['plat1', 'r3_arm1'] }] },

      // Rotating Block 3
      { 
        type: 'rotating', 
        position: [3, 2, -4], 
        color: '#e84393', 
        nodes: [
          { id: 'r3_center', offset: [0, 0.5, 0], connections: ['r3_arm1', 'r3_arm2'] },
          { id: 'r3_arm1', offset: [1, 0.5, 0], connections: ['r3_center', 'plat2'] },
          { id: 'r3_arm2', offset: [-1, 0.5, 0], connections: ['r3_center', 'g1'] }
        ] 
      },

      // Goal Area
      { type: 'cube', position: [1, 2, -4], color: '#ffeaa7', nodes: [{ id: 'g1', offset: [0, 0.5, 0], connections: ['goal', 'r3_arm2'] }] },
      { type: 'cube', position: [0, 2, -4], color: '#ffeaa7', nodes: [{ id: 'goal', offset: [0, 0.5, 0], connections: ['g1'] }] },

      // Decorative Pillars
      { type: 'pillar', position: [6, 0, -2], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [6, 1, -2], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [6, 0, -4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [6, 1, -4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [5, 0, -4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [5, 1, -4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [1, 0, -4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [1, 1, -4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [0, 0, -4], color: '#dfe6e9', nodes: [] },
      { type: 'pillar', position: [0, 1, -4], color: '#dfe6e9', nodes: [] },
    ]  },
  {
    id: 'level-10',
    name: 'The Crystal Maze',
    startNodeId: 's1',
    goalNodeId: 'goal',
    blocks: [
      // Start Area
      { type: 'cube', position: [0, 0, 0], color: '#e91e63', nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['s2'] }] },
      { type: 'cube', position: [1, 0, 0], color: '#e91e63', nodes: [{ id: 's2', offset: [0, 0.5, 0], connections: ['s1', 'p1'] }] },
      
      // Crystal Path
      { type: 'cube', position: [2, 0, 0], color: '#9c27b0', nodes: [{ id: 'p1', offset: [0, 0.5, 0], connections: ['s2', 'p2'] }] },
      { type: 'cube', position: [3, 0, 0], color: '#9c27b0', nodes: [{ id: 'p2', offset: [0, 0.5, 0], connections: ['p1', 'p3'] }] },
      { type: 'cube', position: [4, 0, 0], color: '#9c27b0', nodes: [{ id: 'p3', offset: [0, 0.5, 0], connections: ['p2', 'p4'] }] },
      
      // Illusion Bridge (disconnected in 3D)
      { type: 'cube', position: [4, 3, -3], color: '#00bcd4', nodes: [{ id: 'p4', offset: [0, 0.5, 0], connections: ['p3', 'p5'] }] },
      { type: 'cube', position: [5, 3, -3], color: '#00bcd4', nodes: [{ id: 'p5', offset: [0, 0.5, 0], connections: ['p4', 'p6'] }] },
      { type: 'cube', position: [6, 3, -3], color: '#00bcd4', nodes: [{ id: 'p6', offset: [0, 0.5, 0], connections: ['p5', 'p7'] }] },
      
      // Upper Crystal Chamber
      { type: 'cube', position: [6, 3, -1], color: '#ff9800', nodes: [{ id: 'p7', offset: [0, 0.5, 0], connections: ['p6', 'p8'] }] },
      { type: 'cube', position: [6, 3, 0], color: '#ff9800', nodes: [{ id: 'p8', offset: [0, 0.5, 0], connections: ['p7', 'p9'] }] },
      { type: 'cube', position: [6, 3, 1], color: '#ff9800', nodes: [{ id: 'p9', offset: [0, 0.5, 0], connections: ['p8', 'goal'] }] },
      
      // Goal Chamber
      { type: 'cube', position: [6, 3, 3], color: '#4caf50', nodes: [{ id: 'goal', offset: [0, 0.5, 0], connections: ['p9'] }] },
      
      // Decorative Crystal Pillars
      { type: 'pillar', position: [4, 0, 0], color: '#e91e63', nodes: [] },
      { type: 'pillar', position: [4, 1, 0], color: '#e91e63', nodes: [] },
      { type: 'pillar', position: [4, 2, 0], color: '#e91e63', nodes: [] },
      { type: 'pillar', position: [6, 0, -3], color: '#00bcd4', nodes: [] },
      { type: 'pillar', position: [6, 1, -3], color: '#00bcd4', nodes: [] },
      { type: 'pillar', position: [6, 2, -3], color: '#00bcd4', nodes: [] },
    ]
  },
  {
    id: 'level-11',
    name: 'The Floating Gardens',
    startNodeId: 's1',
    goalNodeId: 'goal',
    blocks: [
      // Ground Level
      { type: 'cube', position: [0, 0, 0], color: '#795548', nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['s2'] }] },
      { type: 'cube', position: [1, 0, 0], color: '#795548', nodes: [{ id: 's2', offset: [0, 0.5, 0], connections: ['s1', 's3'] }] },
      { type: 'cube', position: [2, 0, 0], color: '#795548', nodes: [{ id: 's3', offset: [0, 0.5, 0], connections: ['s2', 'p1'] }] },
      
      // First Floating Platform
      { type: 'cube', position: [2, 2, -2], color: '#8bc34a', nodes: [{ id: 'p1', offset: [0, 0.5, 0], connections: ['s3', 'p2'] }] },
      { type: 'cube', position: [3, 2, -2], color: '#8bc34a', nodes: [{ id: 'p2', offset: [0, 0.5, 0], connections: ['p1', 'p3'] }] },
      { type: 'cube', position: [4, 2, -2], color: '#8bc34a', nodes: [{ id: 'p3', offset: [0, 0.5, 0], connections: ['p2', 'p4'] }] },
      
      // Second Floating Platform (higher)
      { type: 'cube', position: [4, 4, -4], color: '#ffc107', nodes: [{ id: 'p4', offset: [0, 0.5, 0], connections: ['p3', 'p5'] }] },
      { type: 'cube', position: [5, 4, -4], color: '#ffc107', nodes: [{ id: 'p5', offset: [0, 0.5, 0], connections: ['p4', 'p6'] }] },
      { type: 'cube', position: [6, 4, -4], color: '#ffc107', nodes: [{ id: 'p6', offset: [0, 0.5, 0], connections: ['p5', 'goal'] }] },
      
      // Goal Platform
      { type: 'cube', position: [6, 4, -2], color: '#f44336', nodes: [{ id: 'goal', offset: [0, 0.5, 0], connections: ['p6'] }] },
      
      // Supporting Pillars
      { type: 'pillar', position: [2, 0, -2], color: '#607d8b', nodes: [] },
      { type: 'pillar', position: [2, 1, -2], color: '#607d8b', nodes: [] },
      { type: 'pillar', position: [4, 0, -2], color: '#607d8b', nodes: [] },
      { type: 'pillar', position: [4, 1, -2], color: '#607d8b', nodes: [] },
      { type: 'pillar', position: [4, 2, -2], color: '#607d8b', nodes: [] },
      { type: 'pillar', position: [4, 3, -2], color: '#607d8b', nodes: [] },
      { type: 'pillar', position: [6, 0, -4], color: '#607d8b', nodes: [] },
      { type: 'pillar', position: [6, 1, -4], color: '#607d8b', nodes: [] },
      { type: 'pillar', position: [6, 2, -4], color: '#607d8b', nodes: [] },
      { type: 'pillar', position: [6, 3, -4], color: '#607d8b', nodes: [] },
    ]
  },
  {
    id: 'level-12',
    name: 'The Ancient Temple',
    startNodeId: 's1',
    goalNodeId: 'goal',
    blocks: [
      // Temple Entrance
      { type: 'cube', position: [0, 0, 0], color: '#ff5722', nodes: [{ id: 's1', offset: [0, 0.5, 0], connections: ['s2'] }] },
      { type: 'cube', position: [1, 0, 0], color: '#ff5722', nodes: [{ id: 's2', offset: [0, 0.5, 0], connections: ['s1', 's3'] }] },
      { type: 'cube', position: [2, 0, 0], color: '#ff5722', nodes: [{ id: 's3', offset: [0, 0.5, 0], connections: ['s2', 'p1'] }] },
      
      // Inner Chamber
      { type: 'cube', position: [3, 0, 0], color: '#9e9e9e', nodes: [{ id: 'p1', offset: [0, 0.5, 0], connections: ['s3', 'p2'] }] },
      { type: 'cube', position: [4, 0, 0], color: '#9e9e9e', nodes: [{ id: 'p2', offset: [0, 0.5, 0], connections: ['p1', 'p3'] }] },
      { type: 'cube', position: [5, 0, 0], color: '#9e9e9e', nodes: [{ id: 'p3', offset: [0, 0.5, 0], connections: ['p2', 'p4'] }] },
      
      // Hidden Upper Path (Illusion)
      { type: 'cube', position: [5, 2, -2], color: '#673ab7', nodes: [{ id: 'p4', offset: [0, 0.5, 0], connections: ['p3', 'p5'] }] },
      { type: 'cube', position: [6, 2, -2], color: '#673ab7', nodes: [{ id: 'p5', offset: [0, 0.5, 0], connections: ['p4', 'p6'] }] },
      { type: 'cube', position: [7, 2, -2], color: '#673ab7', nodes: [{ id: 'p6', offset: [0, 0.5, 0], connections: ['p5', 'p7'] }] },
      
      // Temple Altar
      { type: 'cube', position: [8, 2, -2], color: '#3f51b5', nodes: [{ id: 'p7', offset: [0, 0.5, 0], connections: ['p6', 'goal'] }] },
      { type: 'cube', position: [9, 2, -2], color: '#3f51b5', nodes: [{ id: 'goal', offset: [0, 0.5, 0], connections: ['p7'] }] },
      
      // Temple Pillars
      { type: 'pillar', position: [3, 0, -1], color: '#795548', nodes: [] },
      { type: 'pillar', position: [3, 1, -1], color: '#795548', nodes: [] },
      { type: 'pillar', position: [5, 0, -1], color: '#795548', nodes: [] },
      { type: 'pillar', position: [5, 1, -1], color: '#795548', nodes: [] },
      { type: 'pillar', position: [7, 0, -2], color: '#795548', nodes: [] },
      { type: 'pillar', position: [7, 1, -2], color: '#795548', nodes: [] },
      { type: 'pillar', position: [9, 0, -2], color: '#795548', nodes: [] },
      { type: 'pillar', position: [9, 1, -2], color: '#795548', nodes: [] },
      
      // Decorative Elements
      { type: 'cube', position: [1, 0, -1], color: '#ff5722', nodes: [] },
      { type: 'cube', position: [1, 0, 1], color: '#ff5722', nodes: [] },
      { type: 'cube', position: [8, 2, -3], color: '#3f51b5', nodes: [] },
      { type: 'cube', position: [8, 2, -1], color: '#3f51b5', nodes: [] },
    ]  }
];
