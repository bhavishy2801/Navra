import React,{ createContext,useContext,useState,useEffect } from 'react';
const ThemeContext=createContext();
export const ThemeProvider=({ children }) => {
  const [isDark,setIsDark]=useState(() => {
    const saved=localStorage.getItem('navra-theme');
    return saved ? saved === 'dark' : true;
  });
  const [themeKey,setThemeKey]=useState(0);
  useEffect(() => {
    document.documentElement.classList.toggle('dark',isDark);
    document.documentElement.setAttribute('data-theme',isDark ? 'dark' : 'light');
    localStorage.setItem('navra-theme',isDark ? 'dark' : 'light');
    setThemeKey(prev => prev+1);
  },[isDark]);
  const toggleTheme=() => setIsDark(prev => !prev);
  return (
    <ThemeContext.Provider value={{ isDark,toggleTheme,themeKey }}>
      {children}
    </ThemeContext.Provider>
  );
};
export const useTheme=() => {
  const context=useContext(ThemeContext);
  if (!context) throw new Error('useTheme must be used within ThemeProvider');
  return context;
};