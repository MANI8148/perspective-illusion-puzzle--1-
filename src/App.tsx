import { useEffect, useRef, useState } from 'react';
import { SceneManager } from './game/SceneManager';
import { LEVELS } from './game/LevelManager';
import { RotateCw, RotateCcw, ChevronRight, ChevronLeft, Trophy, Info } from 'lucide-react';
import { motion, AnimatePresence } from 'motion/react';
import { SoundManager } from './game/SoundManager';

export default function App() {
  const containerRef = useRef<HTMLDivElement>(null);
  const sceneManagerRef = useRef<SceneManager | null>(null);
  const soundManagerRef = useRef<SoundManager>(new SoundManager());
  const [currentLevelIndex, setCurrentLevelIndex] = useState(0);
  const [isGameWon, setIsGameWon] = useState(false);
  const [showTutorial, setShowTutorial] = useState(true);

  useEffect(() => {
    if (!containerRef.current) return;

    const sceneManager = new SceneManager(containerRef.current, soundManagerRef.current);
    sceneManagerRef.current = sceneManager;

    sceneManager.loadLevel(LEVELS[currentLevelIndex]);

    let animationId: number;
    let won = false; // local variable to prevent multiple triggers
    const animate = () => {
      sceneManager.update();
      
      // Check win condition
      const level = LEVELS[currentLevelIndex];
      if (sceneManager.getCurrentNodeId() === level.goalNodeId && !won) {
        won = true;
        setIsGameWon(true);
        soundManagerRef.current.playWin();
      }
      
      animationId = requestAnimationFrame(animate);
    };
    animate();

    return () => {
      cancelAnimationFrame(animationId);
      sceneManager.dispose();
      if (containerRef.current) {
        containerRef.current.innerHTML = '';
      }
    };
  }, [currentLevelIndex]);

  const handleRotate = (dir: number) => {
    sceneManagerRef.current?.rotateCamera(dir);
    soundManagerRef.current.playRotate();
  };

  const nextLevel = () => {
    if (currentLevelIndex < LEVELS.length - 1) {
      setCurrentLevelIndex(currentLevelIndex + 1);
      setIsGameWon(false);
    }
  };

  const prevLevel = () => {
    if (currentLevelIndex > 0) {
      setCurrentLevelIndex(currentLevelIndex - 1);
      setIsGameWon(false);
    }
  };

  return (
    <div className="relative w-full h-screen bg-[#f0f0f0] overflow-hidden font-sans text-[#333]">
      {/* Game Container */}
      <div 
        ref={containerRef} 
        className="relative w-full h-full cursor-pointer"
        onPointerDown={(e) => sceneManagerRef.current?.handlePointerDown(e.nativeEvent, containerRef.current!)}
      />

      {/* UI Overlay */}
      <div className="absolute top-0 left-0 w-full p-6 flex justify-between items-start pointer-events-none">
        <motion.div 
          initial={{ opacity: 0, x: -20 }}
          animate={{ opacity: 1, x: 0 }}
          className="bg-white/80 backdrop-blur-md p-4 rounded-2xl shadow-xl border border-white/20 pointer-events-auto"
        >
          <h1 className="text-xl font-bold tracking-tight text-[#2d3436]">
            {LEVELS[currentLevelIndex].name}
          </h1>
          <p className="text-xs font-medium text-[#636e72] uppercase tracking-widest mt-1">
            Level {currentLevelIndex + 1} of {LEVELS.length}
          </p>
        </motion.div>

        <div className="flex gap-3 pointer-events-auto">
          <button 
            onClick={() => setShowTutorial(!showTutorial)}
            className="p-3 bg-white/80 backdrop-blur-md rounded-full shadow-lg hover:bg-white transition-colors border border-white/20"
          >
            <Info size={20} />
          </button>
        </div>
      </div>

      {/* Controls */}
      <div className="absolute bottom-8 left-1/2 -translate-x-1/2 flex items-center gap-6 p-2 bg-white/40 backdrop-blur-lg rounded-full border border-white/30 shadow-2xl">
        <button 
          onClick={() => handleRotate(-1)}
          className="p-4 bg-white rounded-full shadow-md hover:scale-110 active:scale-95 transition-all text-[#ff6b6b]"
          title="Rotate Left"
        >
          <RotateCcw size={24} />
        </button>
        
        <div className="h-8 w-[1px] bg-black/10" />

        <div className="flex gap-2">
          <button 
            onClick={prevLevel}
            disabled={currentLevelIndex === 0}
            className="p-3 bg-white rounded-full shadow-md hover:scale-110 active:scale-95 transition-all disabled:opacity-30 disabled:hover:scale-100 text-[#2d3436]"
          >
            <ChevronLeft size={20} />
          </button>
          <button 
            onClick={nextLevel}
            disabled={currentLevelIndex === LEVELS.length - 1}
            className="p-3 bg-white rounded-full shadow-md hover:scale-110 active:scale-95 transition-all disabled:opacity-30 disabled:hover:scale-100 text-[#2d3436]"
          >
            <ChevronRight size={20} />
          </button>
        </div>

        <div className="h-8 w-[1px] bg-black/10" />

        <button 
          onClick={() => handleRotate(1)}
          className="p-4 bg-white rounded-full shadow-md hover:scale-110 active:scale-95 transition-all text-[#ff6b6b]"
          title="Rotate Right"
        >
          <RotateCw size={24} />
        </button>
      </div>

      {/* Win Modal */}
      <AnimatePresence>
        {isGameWon && (
          <motion.div 
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            className="absolute inset-0 bg-black/20 backdrop-blur-sm flex items-center justify-center z-50 p-6"
          >
            <motion.div 
              initial={{ scale: 0.8, y: 20 }}
              animate={{ scale: 1, y: 0 }}
              className="bg-white p-8 rounded-[2.5rem] shadow-2xl max-w-sm w-full text-center border border-white/50"
            >
              <div className="w-20 h-20 bg-[#fffae5] rounded-full flex items-center justify-center mx-auto mb-6">
                <Trophy size={40} className="text-[#f1c40f]" />
              </div>
              <h2 className="text-3xl font-bold text-[#2d3436] mb-2">Level Complete!</h2>
              <p className="text-[#636e72] mb-8">You've mastered the illusion of this realm.</p>
              
              {currentLevelIndex < LEVELS.length - 1 ? (
                <button 
                  onClick={nextLevel}
                  className="w-full py-4 bg-[#ff6b6b] text-white rounded-2xl font-bold shadow-lg shadow-red-200 hover:bg-[#ff5252] transition-colors flex items-center justify-center gap-2"
                >
                  Next Level <ChevronRight size={20} />
                </button>
              ) : (
                <p className="text-sm font-bold text-[#ff6b6b] uppercase tracking-widest">
                  More levels coming soon!
                </p>
              )}
            </motion.div>
          </motion.div>
        )}
      </AnimatePresence>

      {/* Tutorial Overlay */}
      <AnimatePresence>
        {showTutorial && (
          <motion.div 
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            className="absolute inset-0 bg-black/40 backdrop-blur-sm flex items-center justify-center z-40 p-6 pointer-events-none"
          >
            <motion.div 
              initial={{ y: 20 }}
              animate={{ y: 0 }}
              className="bg-white/90 p-8 rounded-3xl shadow-2xl max-w-md w-full pointer-events-auto"
            >
              <div className="flex justify-between items-center mb-6">
                <h2 className="text-2xl font-bold">How to Play</h2>
                <button onClick={() => setShowTutorial(false)} className="text-[#636e72] hover:text-black">
                  ✕
                </button>
              </div>
              <ul className="space-y-4 text-[#2d3436]">
                <li className="flex gap-4 items-start">
                  <div className="w-8 h-8 rounded-full bg-[#a8d8ea] flex-shrink-0 flex items-center justify-center text-white font-bold">1</div>
                  <p>Click on platforms or use <b>W, A, S, D</b> keys to move. You can only move to connected paths.</p>
                </li>
                <li className="flex gap-4 items-start">
                  <div className="w-8 h-8 rounded-full bg-[#ffccf9] flex-shrink-0 flex items-center justify-center text-white font-bold">2</div>
                  <p>Rotate the camera to change your perspective. Some paths only exist when they align visually! Use your <b>Mouse Wheel</b> to zoom in and out.</p>
                </li>
                <li className="flex gap-4 items-start">
                  <div className="w-8 h-8 rounded-full bg-[#ff6b6b] flex-shrink-0 flex items-center justify-center text-white font-bold">3</div>
                  <p>Reach the <b>Golden Ring</b> to complete the level.</p>
                </li>
              </ul>
              <button 
                onClick={() => setShowTutorial(false)}
                className="w-full mt-8 py-3 bg-[#2d3436] text-white rounded-xl font-bold hover:bg-black transition-colors"
              >
                Got it!
              </button>
            </motion.div>
          </motion.div>
        )}
      </AnimatePresence>
    </div>
  );
}
