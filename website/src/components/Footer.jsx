import React from 'react';
import { Github, Mail, ExternalLink } from 'lucide-react';
import '../styles/footer.css';
const Footer=() => {
  return (
    <footer className="footer">
      <div className="footer-content">
        <div className="footer-left">
          <div className="footer-brand">NAVRA</div>
          <p className="footer-tagline">Campus Route Optimizer</p>
        </div>
        <div className="footer-links">
        <p>&copy; 2025 NAVRA • DSA Course Project • IIT Jodhpur</p>
      </div>
        <div className="footer-social">
          <a href="https://github.com/bhavishy2801/Navra" target="_blank" rel="noopener noreferrer" aria-label="GitHub">
            <Github size={18} />
          </a>
          <a href="mailto:bhavishyrocker2801@gmail.com" aria-label="Email">
            <Mail size={18} />
          </a>
          <a href="https://drive.google.com/file/d/1xb8m9TSb7B9KDL2uwbSaxff_3AtmJXwl/view?usp=sharing" target="_blank" rel="noopener noreferrer" aria-label="Documentation">
            <ExternalLink size={18} />
          </a>
        </div>
      </div>
    </footer>
  );
};
export default Footer;