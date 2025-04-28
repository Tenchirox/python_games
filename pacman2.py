import tkinter as tk
from tkinter import messagebox
import random
import os
import math
from PIL import Image, ImageTk, ImageDraw

class PacManGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Pac-Man")
        self.root.resizable(False, False)
        
        # Constantes du jeu
        self.WIDTH = 600
        self.HEIGHT = 700
        self.CELL_SIZE = 30
        self.GRID_WIDTH = 19  # Nombre de cellules horizontales
        self.GRID_HEIGHT = 21  # Nombre de cellules verticales
        self.GAME_SPEED = 200  # Millisecondes entre chaque mise à jour (ralenti)
        self.GHOST_SPEED = 0.4  # Vitesse des fantômes (cellules par mise à jour)
        self.PACMAN_SPEED = 1  # Vitesse de Pac-Man (cellules par mise à jour)
        
        # Configuration des niveaux
        self.level_configs = [
            # Niveau 1 - Facile
            {
                "ghost_speed": 0.4,
                "game_speed": 200,
                "power_duration": 10000
            },
            # Niveau 2 - Moyen
            {
                "ghost_speed": 0.5,
                "game_speed": 180,
                "power_duration": 8000,
                "maze_variant": 1
            },
            # Niveau 3 - Difficile
            {
                "ghost_speed": 0.6,
                "game_speed": 160,
                "power_duration": 6000,
                "maze_variant": 2
            }
        ]
        
        # Variables du jeu
        self.score = 0
        self.lives = 3
        self.level = 1
        self.game_over = False
        self.game_started = False
        self.paused = False
        self.direction = "Right"  # Direction initiale de Pac-Man
        self.next_direction = "Right"  # Prochaine direction souhaitée
        self.dots_collected = 0
        self.total_dots = 0
        self.power_mode = False
        self.power_mode_timer = 0
        self.power_mode_duration = 10000  # 10 secondes en mode puissance
        
        # Cr��ation du cadre avec barres de défilement pour le canvas principal
        self.canvas_frame = tk.Frame(root)
        self.canvas_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # Barres de défilement
        self.h_scrollbar = tk.Scrollbar(self.canvas_frame, orient=tk.HORIZONTAL)
        self.h_scrollbar.pack(side=tk.BOTTOM, fill=tk.X)
        
        self.v_scrollbar = tk.Scrollbar(self.canvas_frame, orient=tk.VERTICAL)
        self.v_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # Création du canvas principal avec barres de défilement
        self.canvas = tk.Canvas(self.canvas_frame, width=self.WIDTH, height=self.HEIGHT, bg="black",
                              xscrollcommand=self.h_scrollbar.set, yscrollcommand=self.v_scrollbar.set)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # Configuration des barres de défilement
        self.h_scrollbar.config(command=self.canvas.xview)
        self.v_scrollbar.config(command=self.canvas.yview)
        
        # Configuration de la région de défilement du canvas
        self.canvas.config(scrollregion=(0, 0, self.WIDTH, self.HEIGHT))
        
        # Panneau d'information
        self.info_frame = tk.Frame(root, width=200, height=self.HEIGHT, bg="#2F2F2F")
        self.info_frame.pack(side=tk.RIGHT, fill=tk.BOTH)
        
        # Création du cadre pour les contrôles
        self.control_frame = tk.Frame(self.info_frame, bg="#2F2F2F")
        self.control_frame.pack(pady=10)
        
        # Affichage du score et des vies
        self.score_label = tk.Label(self.info_frame, text="Score: 0", font=("Arial", 14), bg="#2F2F2F", fg="white")
        self.score_label.pack(pady=10)
        
        self.lives_label = tk.Label(self.info_frame, text="Vies: 3", font=("Arial", 14), bg="#2F2F2F", fg="white")
        self.lives_label.pack(pady=10)
        
        self.level_label = tk.Label(self.info_frame, text="Niveau: 1", font=("Arial", 14), bg="#2F2F2F", fg="white")
        self.level_label.pack(pady=10)
        
        # Boutons de contrôle
        self.start_button = tk.Button(self.info_frame, text="Commencer", 
                                    font=("Arial", 12), command=self.start_game)
        self.start_button.pack(pady=10)
        
        self.restart_button = tk.Button(self.info_frame, text="Nouvelle Partie", 
                                      font=("Arial", 12), command=self.reset_game)
        self.restart_button.pack(pady=10)
        self.restart_button.pack_forget()  # Cacher le bouton au démarrage
        
        # Chargement des sprites
        self.load_sprites()
        
        # Initialisation du jeu
        self.init_game()
        
        # Liaison des touches du clavier
        self.setup_bindings()
        
        # Afficher l'écran d'accueil
        self.show_welcome_screen()
    
    def load_sprites(self):
        # Créer un dossier pour les sprites s'il n'existe pas
        sprites_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sprites")
        if not os.path.exists(sprites_dir):
            os.makedirs(sprites_dir)
        
        # Couleurs des éléments du jeu
        self.colors = {
            "pacman": "#FFFF00",  # Jaune
            "ghost1": "#FF0000",  # Rouge
            "ghost2": "#00FFFF",  # Cyan
            "ghost3": "#FFB8FF",  # Rose
            "ghost4": "#FFB852",  # Orange
            "wall": "#0000FF",    # Bleu
            "dot": "#FFFFFF",     # Blanc
            "power": "#FFFFFF"    # Blanc
        }
        
        # Créer les sprites pour chaque élément
        self.sprites = {}
        
        # Pac-Man (cercle jaune avec bouche)
        self.sprites["pacman"] = self.create_pacman_sprite()
        
        # Fantômes
        self.sprites["ghost1"] = self.create_ghost_sprite(self.colors["ghost1"])
        self.sprites["ghost2"] = self.create_ghost_sprite(self.colors["ghost2"])
        self.sprites["ghost3"] = self.create_ghost_sprite(self.colors["ghost3"])
        self.sprites["ghost4"] = self.create_ghost_sprite(self.colors["ghost4"])
        
        # Fantôme vulnérable (bleu)
        self.sprites["ghost_vulnerable"] = self.create_ghost_sprite("#0000FF")
        
        # Mur
        self.sprites["wall"] = self.create_wall_sprite()
        
        # Point
        self.sprites["dot"] = self.create_dot_sprite()
        
        # Super point (power pellet)
        self.sprites["power"] = self.create_power_sprite()
    
    def create_pacman_sprite(self):
        # Créer des sprites pour Pac-Man dans les 4 directions
        size = self.CELL_SIZE - 2
        
        # Dictionnaire pour stocker les sprites dans chaque direction
        pacman_sprites = {}
        
        # Créer un sprite pour chaque direction
        # Droite (par défaut)
        img_right = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img_right)
        draw.ellipse([0, 0, size-1, size-1], fill=self.colors["pacman"])
        draw.polygon([(size//2, size//2), (size, size//4), (size, size*3//4)], fill="black")
        pacman_sprites["Right"] = ImageTk.PhotoImage(img_right)
        
        # Gauche
        img_left = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img_left)
        draw.ellipse([0, 0, size-1, size-1], fill=self.colors["pacman"])
        draw.polygon([(size//2, size//2), (0, size//4), (0, size*3//4)], fill="black")
        pacman_sprites["Left"] = ImageTk.PhotoImage(img_left)
        
        # Haut
        img_up = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img_up)
        draw.ellipse([0, 0, size-1, size-1], fill=self.colors["pacman"])
        draw.polygon([(size//2, size//2), (size//4, 0), (size*3//4, 0)], fill="black")
        pacman_sprites["Up"] = ImageTk.PhotoImage(img_up)
        
        # Bas
        img_down = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img_down)
        draw.ellipse([0, 0, size-1, size-1], fill=self.colors["pacman"])
        draw.polygon([(size//2, size//2), (size//4, size), (size*3//4, size)], fill="black")
        pacman_sprites["Down"] = ImageTk.PhotoImage(img_down)
        
        return pacman_sprites
    
    def create_ghost_sprite(self, color):
        # Créer un sprite pour un fantôme
        size = self.CELL_SIZE - 2
        img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Corps du fantôme (demi-cercle sur rectangle)
        draw.rectangle([0, size//2, size-1, size-1], fill=color)
        draw.ellipse([0, 0, size-1, size], fill=color)
        
        # Base ondulée
        wave_height = size // 6
        wave_width = size // 3
        for i in range(3):
            x1 = i * wave_width
            x2 = (i + 1) * wave_width
            y1 = size
            y2 = size - wave_height
            draw.ellipse([x1, y2, x2, y1 + wave_height], fill="black")
        
        # Yeux
        eye_size = size // 5
        # Œil gauche
        draw.ellipse([size//4 - eye_size//2, size//3 - eye_size//2, 
                     size//4 + eye_size//2, size//3 + eye_size//2], fill="white")
        draw.ellipse([size//4 - eye_size//4, size//3 - eye_size//4, 
                     size//4 + eye_size//4, size//3 + eye_size//4], fill="black")
        # Œil droit
        draw.ellipse([size*3//4 - eye_size//2, size//3 - eye_size//2, 
                     size*3//4 + eye_size//2, size//3 + eye_size//2], fill="white")
        draw.ellipse([size*3//4 - eye_size//4, size//3 - eye_size//4, 
                     size*3//4 + eye_size//4, size//3 + eye_size//4], fill="black")
        
        return ImageTk.PhotoImage(img)
    
    def create_wall_sprite(self):
        # Créer un sprite pour un mur
        size = self.CELL_SIZE
        img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Dessiner le mur (rectangle bleu avec effet 3D)
        draw.rectangle([0, 0, size-1, size-1], fill=self.colors["wall"])
        
        # Effet 3D
        lighter_blue = "#4444FF"
        darker_blue = "#000088"
        
        # Bord supérieur et gauche plus clair
        draw.line([(0, 0), (size-1, 0)], fill=lighter_blue, width=2)  # Haut
        draw.line([(0, 0), (0, size-1)], fill=lighter_blue, width=2)  # Gauche
        
        # Bord inférieur et droit plus foncé
        draw.line([(0, size-1), (size-1, size-1)], fill=darker_blue, width=2)  # Bas
        draw.line([(size-1, 0), (size-1, size-1)], fill=darker_blue, width=2)  # Droite
        
        return ImageTk.PhotoImage(img)
    
    def create_dot_sprite(self):
        # Créer un sprite pour un point
        size = self.CELL_SIZE
        dot_size = size // 5
        img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Dessiner le point (petit cercle blanc)
        x = (size - dot_size) // 2
        y = (size - dot_size) // 2
        draw.ellipse([x, y, x + dot_size, y + dot_size], fill=self.colors["dot"])
        
        return ImageTk.PhotoImage(img)
    
    def create_power_sprite(self):
        # Créer un sprite pour un super point
        size = self.CELL_SIZE
        power_size = size // 2
        img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Dessiner le super point (cercle blanc plus grand)
        x = (size - power_size) // 2
        y = (size - power_size) // 2
        draw.ellipse([x, y, x + power_size, y + power_size], fill=self.colors["power"])
        
        return ImageTk.PhotoImage(img)
    
    def show_welcome_screen(self):
        # Afficher un écran d'accueil avec des instructions
        self.canvas.delete("all")
        
        # Titre du jeu
        self.canvas.create_text(self.WIDTH // 2, self.HEIGHT // 4, 
                              text="PAC-MAN", 
                              font=("Arial", 36, "bold"), 
                              fill="yellow")
        
        # Instructions
        instructions = [
            "Utilisez les flèches pour déplacer Pac-Man",
            "Mangez tous les points pour passer au niveau suivant",
            "Les super points rendent les fantômes vulnérables",
            "Évitez les fantômes ou mangez-les quand ils sont bleus",
            "P pour mettre en pause",
            "\nCliquez sur 'Commencer' pour jouer"
        ]
        
        y_pos = self.HEIGHT // 2
        for line in instructions:
            self.canvas.create_text(self.WIDTH // 2, y_pos, 
                                  text=line, 
                                  font=("Arial", 14), 
                                  fill="white")
            y_pos += 30
        
        # Dessiner quelques éléments décoratifs
        # Pac-Man
        self.canvas.create_image(self.WIDTH // 4, self.HEIGHT * 3 // 4, 
                               image=self.sprites["pacman"][self.direction], anchor="center")
        
        # Fantômes
        ghost_types = ["ghost1", "ghost2", "ghost3", "ghost4"]
        for i, ghost_type in enumerate(ghost_types):
            self.canvas.create_image(self.WIDTH // 4 + (i + 1) * 50, self.HEIGHT * 3 // 4, 
                                   image=self.sprites[ghost_type], anchor="center")
    
    def init_game(self):
        # Initialiser les variables du jeu
        self.score = 0
        self.lives = 3
        self.level = 1
        self.game_over = False
        self.paused = False
        self.direction = "Right"
        self.next_direction = "Right"
        self.dots_collected = 0
        self.total_dots = 0
        self.power_mode = False
        self.power_mode_timer = 0
        
        # Définir la grille de jeu
        self.create_maze()
        
        # Initialiser Pac-Man et les fantômes
        self.init_characters()
    
    def create_maze(self):
        # Créer le labyrinthe du jeu selon le niveau
        # 0 = vide, 1 = mur, 2 = point, 3 = super point, 4 = porte fantôme
        
        # Labyrinthe par défaut (niveau 1)
        maze_variant = 0
        
        # Vérifier si une variante de labyrinthe est spécifiée pour ce niveau
        if self.level <= len(self.level_configs):
            config = self.level_configs[self.level - 1]
            if "maze_variant" in config:
                maze_variant = config["maze_variant"]
        
        # Différentes variantes de labyrinthe
        if maze_variant == 1:
            # Variante 1 - Plus de murs, moins de points de puissance
            self.maze = [
                [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1],
                [1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1],
                [1, 2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 1],
                [1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1],
                [1, 1, 1, 1, 2, 1, 1, 1, 0, 1, 0, 1, 1, 1, 2, 1, 1, 1, 1],
                [0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0],
                [1, 1, 1, 1, 2, 1, 0, 1, 1, 4, 1, 1, 0, 1, 2, 1, 1, 1, 1],
                [0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0],
                [1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1],
                [0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0],
                [1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1],
                [1, 3, 2, 1, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 1, 2, 3, 1],
                [1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1],
                [1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1],
                [1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
            ]
        elif maze_variant == 2:
            # Variante 2 - Labyrinthe plus complexe, plus difficile
            self.maze = [
                [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
                [1, 3, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 3, 1],
                [1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1],
                [1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1],
                [1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1],
                [1, 2, 2, 1, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 1, 2, 2, 1],
                [1, 1, 2, 1, 2, 1, 0, 1, 1, 4, 1, 1, 0, 1, 2, 1, 2, 1, 1],
                [0, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 0],
                [1, 1, 2, 1, 2, 1, 0, 1, 1, 1, 1, 1, 0, 1, 2, 1, 2, 1, 1],
                [1, 2, 2, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 2, 2, 1],
                [1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1],
                [1, 3, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 3, 1],
                [1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
            ]
        else:
            # Labyrinthe par défaut (niveau 1)
            self.maze = [
                [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 3, 1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 3, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 1],
                [1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1],
                [1, 1, 1, 1, 2, 1, 1, 1, 0, 1, 0, 1, 1, 1, 2, 1, 1, 1, 1],
                [0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0],
                [1, 1, 1, 1, 2, 1, 0, 1, 1, 4, 1, 1, 0, 1, 2, 1, 1, 1, 1],
                [0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0],
                [1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1],
                [0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0],
                [1, 1, 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1],
                [1, 3, 2, 1, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 1, 2, 3, 1],
                [1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 1, 1],
                [1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1],
                [1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1],
                [1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1],
                [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
            ]
        
        # Compter le nombre total de points
        self.total_dots = 0
        for row in self.maze:
            for cell in row:
                if cell == 2 or cell == 3:
                    self.total_dots += 1
    
    def init_characters(self):
        # Initialiser Pac-Man
        self.pacman_pos = [15.0, 9.0]  # Utiliser des flottants pour un mouvement fluide
        self.pacman_target = [15, 9]  # Position cible (grille)
        
        # Initialiser les fantômes
        self.ghosts = [
            {"pos": [9.0, 9.0], "target": [9, 9], "type": "ghost1", "direction": "Up", "vulnerable": False},
            {"pos": [9.0, 8.0], "target": [9, 8], "type": "ghost2", "direction": "Up", "vulnerable": False},
            {"pos": [9.0, 10.0], "target": [9, 10], "type": "ghost3", "direction": "Up", "vulnerable": False},
            {"pos": [8.0, 9.0], "target": [8, 9], "type": "ghost4", "direction": "Up", "vulnerable": False}
        ]
        # S'assurer que GHOST_SPEED est défini dans __init__
        self.GHOST_SPEED = self.level_configs[self.level - 1]["ghost_speed"]
    
    def setup_bindings(self):
        # Configurer les touches de contrôle
        self.root.bind("<Left>", lambda event: self.change_direction("Left"))
        self.root.bind("<Right>", lambda event: self.change_direction("Right"))
        self.root.bind("<Up>", lambda event: self.change_direction("Up"))
        self.root.bind("<Down>", lambda event: self.change_direction("Down"))
        self.root.bind("p", self.toggle_pause)
        self.root.bind("P", self.toggle_pause)
    
    def change_direction(self, new_direction):
        # Changer la direction de Pac-Man
        # On stocke la prochaine direction souhaitée
        self.next_direction = new_direction
        
        # Si le jeu est en pause ou terminé, ne pas changer la direction immédiatement
        if self.paused or self.game_over:
            return
            
        # Sinon, on change la direction immédiatement si possible
        self.direction = new_direction
        
    def toggle_pause(self, event=None):
        """Met en pause ou reprend le jeu"""
        if not self.game_started or self.game_over:
            return
        
        self.paused = not self.paused
        
        if self.paused:
            # Afficher un message de pause
            self.canvas.create_text(
                self.WIDTH // 2, self.HEIGHT // 2,
                text="PAUSE",
                font=("Arial", 36, "bold"),
                fill="yellow",
                tags="pause"
            )
        else:
            # Supprimer le message de pause et continuer le jeu
            self.canvas.delete("pause")
            self.update()  # Reprendre la boucle de jeu
    
    def move_pacman(self):
        # Logique de déplacement de Pac-Man
        if self.paused or self.game_over:
            return

        # Calculer la nouvelle position
        new_x, new_y = self.pacman_pos
        if self.direction == 'Left': new_x -= self.PACMAN_SPEED
        elif self.direction == 'Right': new_x += self.PACMAN_SPEED
        elif self.direction == 'Up': new_y -= self.PACMAN_SPEED
        elif self.direction == 'Down': new_y += self.PACMAN_SPEED

        # Vérifier les collisions avec les murs
        grid_x, grid_y = int(round(new_x)), int(round(new_y))
        if 0 <= grid_x < self.GRID_HEIGHT and 0 <= grid_y < self.GRID_WIDTH:
            if self.maze[grid_x][grid_y] not in [1, 4]:
                self.pacman_pos = [new_x, new_y]

        # Gérer la téléportation aux bords
        if self.pacman_pos[1] < 0:
            self.pacman_pos[1] = self.GRID_WIDTH - 1
        elif self.pacman_pos[1] >= self.GRID_WIDTH:
            self.pacman_pos[1] = 0

    def get_valid_directions(self, pos):
        valid = []
        directions = {'Up': [-1, 0], 'Down': [1, 0], 'Left': [0, -1], 'Right': [0, 1]}
        for direction, (dx, dy) in directions.items():
            new_x, new_y = pos[0] + dx, pos[1] + dy
            if 0 <= new_x < self.GRID_HEIGHT and 0 <= new_y < self.GRID_WIDTH:
                if self.maze[new_x][new_y] not in [1, 4]:
                    valid.append(direction)
        return valid

    def get_direction_to_target(self, ghost_pos, target_pos):
        dx = target_pos[0] - ghost_pos[0]
        dy = target_pos[1] - ghost_pos[1]
        if abs(dx) > abs(dy):
            return 'Down' if dx > 0 else 'Up'
        else:
            return 'Right' if dy > 0 else 'Left'

    def move_ghosts(self):
        """Déplace les fantômes dans le labyrinthe"""
        import random  # Assurez-vous que random est importé
        
        pacman_grid_pos = (int(round(self.pacman_pos[0])), int(round(self.pacman_pos[1])))

        for ghost in self.ghosts:
            # Mettre à jour l'état de vulnérabilité du fantôme
            ghost["vulnerable"] = self.power_mode
            
            # Utiliser la position arrondie pour la logique de grille
            current_grid_pos = (int(round(ghost["pos"][0])), int(round(ghost["pos"][1])))
            
            # Vérifier si le fantôme est exactement sur une case de la grille pour prendre une décision
            if abs(ghost["pos"][0] - ghost["target"][0]) < 0.1 and abs(ghost["pos"][1] - ghost["target"][1]) < 0.1:
                # Le fantôme a atteint sa case cible, il peut décider de sa prochaine direction
                ghost["pos"] = list(ghost["target"])  # Aligner précisément sur la grille
                current_grid_pos = ghost["target"]  # Mettre à jour la position de grille actuelle

                valid_directions = self.get_valid_directions(current_grid_pos)
                
                # Empêcher le demi-tour immédiat
                opposite_direction = {"Up": "Down", "Down": "Up", "Left": "Right", "Right": "Left"}
                current_direction = ghost["direction"]
                if len(valid_directions) > 1 and opposite_direction[current_direction] in valid_directions:
                    valid_directions.remove(opposite_direction[current_direction])

                # Logique de décision basée sur le type de fantôme et l'état
                is_vulnerable = ghost["vulnerable"]
                
                # Logique de décision basée sur l'état et le type de fantôme
                if is_vulnerable:
                    # En mode vulnérable, s'éloigner de Pac-Man
                    if valid_directions:
                        # Choisir la direction qui éloigne le plus de Pac-Man
                        best_direction = None
                        max_distance = -1
                        
                        for direction in valid_directions:
                            next_r, next_c = current_grid_pos
                            if direction == "Up": next_r -= 1
                            elif direction == "Down": next_r += 1
                            elif direction == "Left": next_c -= 1
                            elif direction == "Right": next_c += 1
                            
                            # Gérer le tunnel
                            if next_c < 0: next_c = self.GRID_WIDTH - 1
                            elif next_c >= self.GRID_WIDTH: next_c = 0
                                
                            distance = math.sqrt((next_r - pacman_grid_pos[0])**2 + (next_c - pacman_grid_pos[1])**2)
                            
                            if distance > max_distance:
                                max_distance = distance
                                best_direction = direction
                        
                        if best_direction:
                            ghost["direction"] = best_direction
                        else:
                            # Si aucune direction n'est trouvée, choisir aléatoirement
                            ghost["direction"] = random.choice(valid_directions) if valid_directions else current_direction
                # Si pas vulnérable, comportement basé sur le type de fantôme
                elif ghost["type"] in ["ghost1", "ghost2"]:  # Rouge et Cyan - poursuite directe
                    if valid_directions:
                        # Choisir la direction qui rapproche le plus de Pac-Man
                        best_direction = None
                        min_distance = float('inf')
                        
                        for direction in valid_directions:
                            next_r, next_c = current_grid_pos
                            if direction == "Up": next_r -= 1
                            elif direction == "Down": next_r += 1
                            elif direction == "Left": next_c -= 1
                            elif direction == "Right": next_c += 1
                            
                            # Gérer le tunnel
                            if next_c < 0: next_c = self.GRID_WIDTH - 1
                            elif next_c >= self.GRID_WIDTH: next_c = 0
                                
                            distance = math.sqrt((next_r - pacman_grid_pos[0])**2 + (next_c - pacman_grid_pos[1])**2)
                            
                            if distance < min_distance:
                                min_distance = distance
                                best_direction = direction
                        
                        if best_direction:
                            ghost["direction"] = best_direction
                        else:
                            ghost["direction"] = random.choice(valid_directions) if valid_directions else current_direction
                elif ghost["type"] == "ghost3":  # Rose - tente de se positionner devant Pac-Man
                    if valid_directions:
                        # Prédire où Pac-Man sera dans quelques pas
                        target_pos = list(pacman_grid_pos)
                        if self.direction == "Up": target_pos[0] -= 4
                        elif self.direction == "Down": target_pos[0] += 4
                        elif self.direction == "Left": target_pos[1] -= 4
                        elif self.direction == "Right": target_pos[1] += 4
                        
                        # Choisir la direction qui rapproche le plus de cette position cible
                        best_direction = None
                        min_distance = float('inf')
                        
                        for direction in valid_directions:
                            next_r, next_c = current_grid_pos
                            if direction == "Up": next_r -= 1
                            elif direction == "Down": next_r += 1
                            elif direction == "Left": next_c -= 1
                            elif direction == "Right": next_c += 1
                            
                            # Gérer le tunnel
                            if next_c < 0: next_c = self.GRID_WIDTH - 1
                            elif next_c >= self.GRID_WIDTH: next_c = 0
                                
                            distance = math.sqrt((next_r - target_pos[0])**2 + (next_c - target_pos[1])**2)
                            
                            if distance < min_distance:
                                min_distance = distance
                                best_direction = direction
                        
                        if best_direction:
                            ghost["direction"] = best_direction
                        else:
                            ghost["direction"] = random.choice(valid_directions) if valid_directions else current_direction
                else:  # Orange - comportement semi-aléatoire
                    if valid_directions:
                        # 70% de chance de poursuivre, 30% de chance d'être aléatoire
                        if random.random() < 0.7:
                            # Choisir la direction qui rapproche le plus de Pac-Man
                            best_direction = None
                            min_distance = float('inf')
                            
                            for direction in valid_directions:
                                next_r, next_c = current_grid_pos
                                if direction == "Up": next_r -= 1
                                elif direction == "Down": next_r += 1
                                elif direction == "Left": next_c -= 1
                                elif direction == "Right": next_c += 1
                                
                                # Gérer le tunnel
                                if next_c < 0: next_c = self.GRID_WIDTH - 1
                                elif next_c >= self.GRID_WIDTH: next_c = 0
                                    
                                distance = math.sqrt((next_r - pacman_grid_pos[0])**2 + (next_c - pacman_grid_pos[1])**2)
                                
                                if distance < min_distance:
                                    min_distance = distance
                                    best_direction = direction
                            
                            if best_direction:
                                ghost["direction"] = best_direction
                            else:
                                ghost["direction"] = random.choice(valid_directions) if valid_directions else current_direction
                        else:
                            ghost["direction"] = random.choice(valid_directions) if valid_directions else current_direction

                # Calculer la nouvelle position cible (prochaine case de la grille)
                next_target_r, next_target_c = current_grid_pos
                if ghost["direction"] == "Up": next_target_r -= 1
                elif ghost["direction"] == "Down": next_target_r += 1
                elif ghost["direction"] == "Left": next_target_c -= 1
                elif ghost["direction"] == "Right": next_target_c += 1

                # Gérer le tunnel pour la cible
                if next_target_c < 0: next_target_c = self.GRID_WIDTH - 1
                elif next_target_c >= self.GRID_WIDTH: next_target_c = 0
                
                ghost["target"] = [next_target_r, next_target_c]

            # Mouvement fluide vers la cible
            target_r, target_c = ghost["target"]
            current_r, current_c = ghost["pos"]
            move_speed = self.GHOST_SPEED
            
            # Calculer le déplacement pour ce tick
            delta_r = target_r - current_r
            delta_c = target_c - current_c
            
            # Appliquer la vitesse
            move_r = 0
            move_c = 0
            
            if abs(delta_r) > 0.01:  # Mouvement vertical
                move_r = math.copysign(min(move_speed, abs(delta_r)), delta_r)
            if abs(delta_c) > 0.01:  # Mouvement horizontal
                move_c = math.copysign(min(move_speed, abs(delta_c)), delta_c)
            
            ghost["pos"][0] += move_r
            ghost["pos"][1] += move_c

    def get_valid_directions(self, pos):
        """Retourne les directions valides depuis une position donnée"""
        row, col = int(round(pos[0])), int(round(pos[1]))
        directions = []
        possible_moves = {
            "Up": (row - 1, col),
            "Down": (row + 1, col),
            "Left": (row, col - 1),
            "Right": (row, col + 1)
        }

        for direction, (next_r, next_c) in possible_moves.items():
            # Gérer le tunnel
            if next_c < 0: next_c = self.GRID_WIDTH - 1
            elif next_c >= self.GRID_WIDTH: next_c = 0

            # Vérifier les limites du labyrinthe
            if 0 <= next_r < self.GRID_HEIGHT:
                cell_type = self.maze[next_r][next_c]
                # Les fantômes peuvent traverser les points (2), super points (3) et les espaces vides (0)
                # Ils ne peuvent pas traverser les murs (1)
                if cell_type != 1:
                    directions.append(direction)
                    
        return directions

    def check_collisions(self):
        """Vérifie les collisions entre Pac-Man, les points et les fantômes"""
        # Conversion de la position de Pac-Man en indices de grille
        pac_x, pac_y = int(round(self.pacman_pos[0])), int(round(self.pacman_pos[1]))
        
        # Sécurité pour éviter les erreurs d'index
        if 0 <= pac_x < self.GRID_HEIGHT and 0 <= pac_y < self.GRID_WIDTH:
            cell = self.maze[pac_x][pac_y]
            if cell == 2:  # Pièce normale
                self.maze[pac_x][pac_y] = 0
                self.score += 10
                self.dots_collected += 1
                self.update_score()
            elif cell == 3:  # Super point (inverseur)
                self.maze[pac_x][pac_y] = 0
                self.score += 50
                self.dots_collected += 1
                self.update_score()
                self.activate_power_mode()

        # Collision avec les fantômes
        for ghost in self.ghosts:
            # Utiliser une détection de collision plus précise basée sur la distance euclidienne
            distance = math.sqrt((self.pacman_pos[0] - ghost["pos"][0])**2 + 
                               (self.pacman_pos[1] - ghost["pos"][1])**2)
            
            # Si la distance est inférieure à un seuil, il y a collision
            if distance < 0.8:  # Seuil de collision ajusté
                if ghost["vulnerable"] and self.power_mode:
                    # Manger un fantôme vulnérable
                    self.score += 200
                    self.update_score()
                    # Réinitialiser la position du fantôme
                    ghost["pos"] = [9.0, 9.0]
                    ghost["target"] = [9, 9]
                    ghost["vulnerable"] = False  # N'est plus vulnérable après avoir été mangé
                elif not self.power_mode:  # Pac-Man perd une vie seulement si pas en mode puissance
                    self.lose_life()
                    return  # Sortir après avoir perdu une vie pour éviter multi-collisions
        
        # Vérifier si tous les points sont collectés
        if self.dots_collected >= self.total_dots:
            self.next_level()
    
    def update_score(self):
        """Met à jour l'affichage du score"""
        self.score_label.config(text=f"Score: {self.score}")
    
    def activate_power_mode(self):
        """Active le mode puissance (fantômes vulnérables)"""
        self.power_mode = True
        self.power_mode_timer = self.level_configs[self.level - 1]["power_duration"]
        for ghost in self.ghosts:
            ghost["vulnerable"] = True
    
    def lose_life(self):
        """Gère la perte d'une vie"""
        self.lives -= 1
        self.update_lives()
        if self.lives <= 0:
            self.game_over = True
            self.show_game_over()
        else:
            self.init_characters()
    
    def update_lives(self):
        """Met à jour l'affichage des vies"""
        self.lives_label.config(text=f"Vies: {self.lives}")

    def start_game(self):
        """Démarre le jeu"""
        if not self.game_started:
            self.game_started = True
            self.start_button.pack_forget()  # Cacher le bouton de démarrage
            self.update()  # Démarrer la boucle de jeu

    def update(self):
        """Met à jour l'état du jeu à chaque tick"""
        if self.game_over or self.paused or not self.game_started:
            return
        
        # Mettre à jour le mode puissance
        if self.power_mode:
            self.power_mode_timer -= self.GAME_SPEED
            if self.power_mode_timer <= 0:
                self.power_mode = False
                for ghost in self.ghosts:
                    ghost["vulnerable"] = False
        
        # Déplacer Pac-Man
        self.move_pacman()
        
        # Déplacer les fantômes
        self.move_ghosts()
        
        # Vérifier les collisions
        self.check_collisions()
        
        # Dessiner le labyrinthe
        self.draw_maze()
        
        # Planifier la prochaine mise à jour
        self.root.after(self.GAME_SPEED, self.update)

    def draw_maze(self):
        # Dessiner le labyrinthe et tous les éléments du jeu
        self.canvas.delete("all")
        
        # Calculer la taille totale du labyrinthe
        maze_width = self.GRID_WIDTH * self.CELL_SIZE
        maze_height = self.GRID_HEIGHT * self.CELL_SIZE
        
        # Mettre à jour la région de défilement du canvas
        self.canvas.config(scrollregion=(0, 0, maze_width, maze_height))
        
        # Dessiner les murs, points et super points
        for i in range(self.GRID_HEIGHT):
            for j in range(self.GRID_WIDTH):
                x = j * self.CELL_SIZE
                y = i * self.CELL_SIZE
                
                cell_type = self.maze[i][j]
                if cell_type == 1:  # Mur
                    self.canvas.create_image(x + self.CELL_SIZE//2, y + self.CELL_SIZE//2, 
                                           image=self.sprites["wall"])
                elif cell_type == 2:  # Point
                    self.canvas.create_image(x + self.CELL_SIZE//2, y + self.CELL_SIZE//2, 
                                           image=self.sprites["dot"])
                elif cell_type == 3:  # Super point
                    self.canvas.create_image(x + self.CELL_SIZE//2, y + self.CELL_SIZE//2, 
                                           image=self.sprites["power"])
                elif cell_type == 4:  # Porte fantôme
                    self.canvas.create_rectangle(x, y + self.CELL_SIZE//2 - 2, 
                                              x + self.CELL_SIZE, y + self.CELL_SIZE//2 + 2, 
                                              fill="#FF69B4")
        
        # Dessiner Pac-Man
        x = self.pacman_pos[1] * self.CELL_SIZE + self.CELL_SIZE//2
        y = self.pacman_pos[0] * self.CELL_SIZE + self.CELL_SIZE//2
        self.canvas.create_image(x, y, image=self.sprites["pacman"][self.direction])
        
        # Dessiner les fantômes
        for ghost in self.ghosts:
            x = ghost["pos"][1] * self.CELL_SIZE + self.CELL_SIZE//2
            y = ghost["pos"][0] * self.CELL_SIZE + self.CELL_SIZE//2
            
            # Utiliser le sprite vulnérable si en mode puissance
            if ghost["vulnerable"] and self.power_mode:
                self.canvas.create_image(x, y, image=self.sprites["ghost_vulnerable"])
            else:
                self.canvas.create_image(x, y, image=self.sprites[ghost["type"]])
        
        # Centrer la vue sur Pac-Man pour le suivre
        self.center_view_on_pacman()
        
        # Vérifier les collisions avec les points et les fantômes
        self.check_collisions()
        
        # Mettre à jour les informations affichées
        self.score_label.config(text=f"Score: {self.score}")
        self.lives_label.config(text=f"Vies: {self.lives}")
        self.level_label.config(text=f"Niveau: {self.level}")
        
    def center_view_on_pacman(self):
        # Centrer la vue du canvas sur Pac-Man
        x = self.pacman_pos[1] * self.CELL_SIZE + self.CELL_SIZE//2
        y = self.pacman_pos[0] * self.CELL_SIZE + self.CELL_SIZE//2
        
        # Calculer les coordonnées pour centrer la vue
        canvas_width = self.canvas.winfo_width()
        canvas_height = self.canvas.winfo_height()
        
        # Si le canvas n'est pas encore rendu, utiliser les dimensions définies
        if canvas_width <= 1:
            canvas_width = self.WIDTH
        if canvas_height <= 1:
            canvas_height = self.HEIGHT
        
        # Calculer les fractions pour le défilement
        x_fraction = (x - canvas_width/2) / (self.GRID_WIDTH * self.CELL_SIZE)
        y_fraction = (y - canvas_height/2) / (self.GRID_HEIGHT * self.CELL_SIZE)
        
        # Limiter les fractions entre 0 et 1
        x_fraction = max(0, min(1, x_fraction))
        y_fraction = max(0, min(1, y_fraction))
        
        # Appliquer le défilement
        self.canvas.xview_moveto(x_fraction)
        self.canvas.yview_moveto(y_fraction)
    
    def next_level(self):
        # Passer au niveau suivant
        self.level += 1
        
        # Réinitialiser le jeu pour le nouveau niveau
        self.dots_collected = 0
        self.power_mode = False
        
        # Créer un nouveau labyrinthe
        self.create_maze()
        
        # Réinitialiser les positions
        self.pacman_pos = [15.0, 9.0]
        self.pacman_target = [15, 9]
        for ghost in self.ghosts:
            ghost["pos"] = [9.0, 9.0]
            ghost["target"] = [9, 9]
            ghost["vulnerable"] = False
        
        # Mettre à jour la vitesse des fantômes et la durée du mode puissance si configurées
        if self.level <= len(self.level_configs):
            config = self.level_configs[self.level - 1]
            self.GHOST_SPEED = config["ghost_speed"]
            self.GAME_SPEED = config["game_speed"]
            self.power_mode_duration = config["power_duration"]
        
        # Mettre à jour l'affichage du niveau
        self.level_label.config(text=f"Niveau: {self.level}")
        
        # Afficher un message de niveau suivant
        self.canvas.delete("all")
        self.canvas.create_text(self.WIDTH // 2, self.HEIGHT // 2, 
                              text=f"Niveau {self.level}", 
                              font=("Arial", 36, "bold"), 
                              fill="yellow")
        self.root.update()
        self.root.after(2000)  # Pause de 2 secondes pour montrer le message
    
    def show_game_over_screen(self):
        """Affiche l'écran de fin de jeu"""
        self.game_over = True
        self.show_game_over_screen()
        self.restart_button.pack(pady=10)
        self.start_button.pack_forget()

# Fonction principale pour démarrer le jeu
def main():
    root = tk.Tk()
    game = PacManGame(root)
    root.mainloop()

if __name__ == "__main__":
    main()
