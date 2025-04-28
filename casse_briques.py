import tkinter as tk
from tkinter import messagebox
import random
import os
from PIL import Image, ImageTk, ImageDraw

class BrickGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Jeu de Casse-Briques")
        self.root.resizable(False, False)
        
        # Constantes du jeu
        self.WIDTH = 800
        self.HEIGHT = 600
        self.PADDLE_WIDTH = 100
        self.PADDLE_HEIGHT = 20
        self.BALL_RADIUS = 10
        self.BRICK_WIDTH = 75
        self.BRICK_HEIGHT = 30
        self.BRICK_ROWS = 5
        self.BRICK_COLS = 10
        self.BRICK_PADDING = 5
        self.GAME_SPEED = 16  # Millisecondes entre chaque mise à jour (environ 60 FPS)
        self.POWERUP_SIZE = 30  # Taille des bonus/malus
        self.POWERUP_SPEED = 3  # Vitesse de chute des bonus/malus
        self.POWERUP_DURATION_BONUS = 20000  # Durée des bonus en millisecondes (20 secondes)
        self.POWERUP_DURATION_MALUS = 10000  # Durée des malus en millisecondes (10 secondes)
        
        # Variables du jeu
        self.score = 0
        self.lives = 3
        self.level = 1
        self.game_over = False
        self.game_started = False
        self.ball_speed_x = 4.5
        self.ball_speed_y = -4.5
        self.ball_moving = False
        self.powerups = []  # Liste des bonus/malus actifs
        self.active_effects = {}  # Effets actifs avec leur temps d'expiration
        
        # Création du canvas principal
        self.canvas = tk.Canvas(root, width=self.WIDTH, height=self.HEIGHT, bg="black")
        self.canvas.pack()
        
        # Création du cadre pour les contrôles
        self.control_frame = tk.Frame(root)
        self.control_frame.pack(pady=10)
        
        # Affichage du score et des vies
        self.score_label = tk.Label(self.control_frame, text="Score: 0", font=("Arial", 14))
        self.score_label.pack(side=tk.LEFT, padx=20)
        
        self.lives_label = tk.Label(self.control_frame, text="Vies: 3", font=("Arial", 14))
        self.lives_label.pack(side=tk.LEFT, padx=20)
        
        self.level_label = tk.Label(self.control_frame, text="Niveau: 1", font=("Arial", 14))
        self.level_label.pack(side=tk.LEFT, padx=20)
        
        # Bouton pour démarrer/redémarrer le jeu
        self.start_button = tk.Button(self.control_frame, text="Commencer", 
                                     font=("Arial", 12), command=self.start_game)
        self.start_button.pack(side=tk.LEFT, padx=20)
        
        # Bouton pour redémarrer le jeu (initialement caché)
        self.restart_button = tk.Button(self.control_frame, text="Nouvelle Partie", 
                                      font=("Arial", 12), command=self.reset_game)
        self.restart_button.pack(side=tk.LEFT, padx=20)
        self.restart_button.pack_forget()  # Cacher le bouton au démarrage
        
        # Chargement des sprites
        self.load_sprites()
        
        # Initialisation des éléments du jeu
        self.init_game()
        
        # Liaison des touches du clavier et de la souris
        self.root.bind("<KeyPress>", self.key_press)
        self.root.bind("<Motion>", self.move_paddle)
        
        # Afficher l'écran d'accueil
        self.show_welcome_screen()
    
    def load_sprites(self):
        # Créer un dossier pour les sprites s'il n'existe pas
        sprites_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sprites")
        if not os.path.exists(sprites_dir):
            os.makedirs(sprites_dir)
        
        # Créer des sprites pour les éléments du jeu
        # Balle
        ball_img = Image.new("RGBA", (self.BALL_RADIUS*2, self.BALL_RADIUS*2), (0, 0, 0, 0))
        ball_draw = ImageDraw.Draw(ball_img)
        ball_draw.ellipse([0, 0, self.BALL_RADIUS*2-1, self.BALL_RADIUS*2-1], fill="#FFFFFF")
        # Ajouter un effet de brillance
        ball_draw.ellipse([self.BALL_RADIUS//2, self.BALL_RADIUS//2, 
                          self.BALL_RADIUS, self.BALL_RADIUS], fill="#FFFFFF")
        self.ball_sprite = ImageTk.PhotoImage(ball_img)
        
        # Raquette
        paddle_img = Image.new("RGBA", (self.PADDLE_WIDTH, self.PADDLE_HEIGHT), (0, 0, 0, 0))
        paddle_draw = ImageDraw.Draw(paddle_img)
        # Dessiner la raquette avec un dégradé
        for y in range(self.PADDLE_HEIGHT):
            # Créer un dégradé du bleu clair au bleu foncé
            blue_val = 200 - int(y * 100 / self.PADDLE_HEIGHT)
            paddle_draw.line([(0, y), (self.PADDLE_WIDTH, y)], fill=(0, 100, blue_val))
        # Ajouter des détails
        paddle_draw.rectangle([0, 0, self.PADDLE_WIDTH, self.PADDLE_HEIGHT], outline="#4040FF", width=2)
        self.paddle_sprite = ImageTk.PhotoImage(paddle_img)
        
        # Briques (différentes couleurs selon les rangées)
        self.brick_sprites = []
        colors = ["#FF0000", "#FF7F00", "#FFFF00", "#00FF00", "#0000FF", "#4B0082", "#9400D3"]
        
        for color in colors[:self.BRICK_ROWS]:
            brick_img = Image.new("RGBA", (self.BRICK_WIDTH, self.BRICK_HEIGHT), (0, 0, 0, 0))
            brick_draw = ImageDraw.Draw(brick_img)
            
            # Dessiner la brique avec un effet 3D
            brick_draw.rectangle([0, 0, self.BRICK_WIDTH-1, self.BRICK_HEIGHT-1], fill=color)
            # Bord supérieur et gauche plus clair (effet de lumière)
            lighter_color = self.lighten_color(color)
            brick_draw.line([(0, 0), (self.BRICK_WIDTH-1, 0)], fill=lighter_color, width=2)  # Haut
            brick_draw.line([(0, 0), (0, self.BRICK_HEIGHT-1)], fill=lighter_color, width=2)  # Gauche
            # Bord inférieur et droit plus foncé (effet d'ombre)
            darker_color = self.darken_color(color)
            brick_draw.line([(0, self.BRICK_HEIGHT-1), (self.BRICK_WIDTH-1, self.BRICK_HEIGHT-1)], 
                            fill=darker_color, width=2)  # Bas
            brick_draw.line([(self.BRICK_WIDTH-1, 0), (self.BRICK_WIDTH-1, self.BRICK_HEIGHT-1)], 
                            fill=darker_color, width=2)  # Droite
            
            self.brick_sprites.append(ImageTk.PhotoImage(brick_img))
        
        # Créer les sprites pour les bonus et malus
        self.powerup_sprites = {}
        
        # Bonus (vert)
        # 1. Agrandir la raquette
        pu_img = Image.new("RGBA", (self.POWERUP_SIZE, self.POWERUP_SIZE), (0, 0, 0, 0))
        pu_draw = ImageDraw.Draw(pu_img)
        pu_draw.ellipse([0, 0, self.POWERUP_SIZE-1, self.POWERUP_SIZE-1], fill="#00FF00")
        pu_draw.line([(self.POWERUP_SIZE//2, 4), (self.POWERUP_SIZE//2, self.POWERUP_SIZE-4)], fill="white", width=2)
        pu_draw.line([(4, self.POWERUP_SIZE//2), (self.POWERUP_SIZE-4, self.POWERUP_SIZE//2)], fill="white", width=2)
        self.powerup_sprites["paddle_grow"] = ImageTk.PhotoImage(pu_img)
        
        # 2. Ralentir la balle
        pu_img = Image.new("RGBA", (self.POWERUP_SIZE, self.POWERUP_SIZE), (0, 0, 0, 0))
        pu_draw = ImageDraw.Draw(pu_img)
        pu_draw.ellipse([0, 0, self.POWERUP_SIZE-1, self.POWERUP_SIZE-1], fill="#00FF00")
        pu_draw.polygon([(4, self.POWERUP_SIZE//2), (self.POWERUP_SIZE-4, 4), (self.POWERUP_SIZE-4, self.POWERUP_SIZE-4)], fill="white")
        self.powerup_sprites["ball_slow"] = ImageTk.PhotoImage(pu_img)
        
        # 3. Vie supplémentaire
        pu_img = Image.new("RGBA", (self.POWERUP_SIZE, self.POWERUP_SIZE), (0, 0, 0, 0))
        pu_draw = ImageDraw.Draw(pu_img)
        pu_draw.ellipse([0, 0, self.POWERUP_SIZE-1, self.POWERUP_SIZE-1], fill="#00FF00")
        # Dessiner un cœur
        heart_points = [
            (self.POWERUP_SIZE//2, 5),
            (self.POWERUP_SIZE-5, self.POWERUP_SIZE//3),
            (self.POWERUP_SIZE//2, self.POWERUP_SIZE-5),
            (5, self.POWERUP_SIZE//3)
        ]
        pu_draw.polygon(heart_points, fill="white")
        self.powerup_sprites["extra_life"] = ImageTk.PhotoImage(pu_img)
        
        # Malus (rouge)
        # 1. Rétrécir la raquette
        pu_img = Image.new("RGBA", (self.POWERUP_SIZE, self.POWERUP_SIZE), (0, 0, 0, 0))
        pu_draw = ImageDraw.Draw(pu_img)
        pu_draw.ellipse([0, 0, self.POWERUP_SIZE-1, self.POWERUP_SIZE-1], fill="#FF0000")
        pu_draw.line([(4, self.POWERUP_SIZE//2), (self.POWERUP_SIZE-4, self.POWERUP_SIZE//2)], fill="white", width=2)
        self.powerup_sprites["paddle_shrink"] = ImageTk.PhotoImage(pu_img)
        
        # 2. Accélérer la balle
        pu_img = Image.new("RGBA", (self.POWERUP_SIZE, self.POWERUP_SIZE), (0, 0, 0, 0))
        pu_draw = ImageDraw.Draw(pu_img)
        pu_draw.ellipse([0, 0, self.POWERUP_SIZE-1, self.POWERUP_SIZE-1], fill="#FF0000")
        pu_draw.polygon([(4, 4), (4, self.POWERUP_SIZE-4), (self.POWERUP_SIZE-4, self.POWERUP_SIZE//2)], fill="white")
        self.powerup_sprites["ball_fast"] = ImageTk.PhotoImage(pu_img)
    
    def lighten_color(self, color_hex):
        # Convertir la couleur hex en RGB et la rendre plus claire
        r = int(color_hex[1:3], 16)
        g = int(color_hex[3:5], 16)
        b = int(color_hex[5:7], 16)
        
        # Éclaircir
        r = min(255, r + 50)
        g = min(255, g + 50)
        b = min(255, b + 50)
        
        return f"#{r:02x}{g:02x}{b:02x}"
    
    def darken_color(self, color_hex):
        # Convertir la couleur hex en RGB et la rendre plus foncée
        r = int(color_hex[1:3], 16)
        g = int(color_hex[3:5], 16)
        b = int(color_hex[5:7], 16)
        
        # Assombrir
        r = max(0, r - 50)
        g = max(0, g - 50)
        b = max(0, b - 50)
        
        return f"#{r:02x}{g:02x}{b:02x}"
    
    def show_welcome_screen(self):
        # Afficher un écran d'accueil avec des instructions
        self.canvas.delete("all")
        
        # Titre du jeu
        self.canvas.create_text(self.WIDTH // 2, self.HEIGHT // 4, 
                              text="JEU DE CASSE-BRIQUES", 
                              font=("Arial", 36, "bold"), 
                              fill="white")
        
        # Instructions
        instructions = [
            "Utilisez la souris pour déplacer la raquette",
            "Détruisez toutes les briques pour passer au niveau suivant",
            "Vous avez 3 vies pour terminer le jeu",
            "\nCliquez sur 'Commencer' pour jouer"
        ]
        
        y_pos = self.HEIGHT // 2
        for line in instructions:
            self.canvas.create_text(self.WIDTH // 2, y_pos, 
                                  text=line, 
                                  font=("Arial", 16), 
                                  fill="white")
            y_pos += 30
        
        # Dessiner quelques éléments décoratifs
        # Raquette
        self.canvas.create_image(self.WIDTH // 2, self.HEIGHT * 3 // 4, 
                               image=self.paddle_sprite, anchor="center")
        
        # Balle
        self.canvas.create_image(self.WIDTH // 2, self.HEIGHT * 3 // 4 - 30, 
                               image=self.ball_sprite, anchor="center")
        
        # Quelques briques
        for i in range(5):
            self.canvas.create_image(self.WIDTH // 2 - 150 + i * 75, self.HEIGHT // 3, 
                                   image=self.brick_sprites[i % len(self.brick_sprites)], 
                                   anchor="center")
    
    def init_game(self):
        # Initialiser les éléments du jeu sans démarrer
        self.score = 0
        self.lives = 3
        self.level = 1
        self.score_label.config(text=f"Score: {self.score}")
        self.lives_label.config(text=f"Vies: {self.lives}")
        self.level_label.config(text=f"Niveau: {self.level}")
        
        # Position initiale de la raquette
        self.paddle_x = self.WIDTH // 2 - self.PADDLE_WIDTH // 2
        self.paddle_y = self.HEIGHT - 50
        
        # Position initiale de la balle (sur la raquette)
        self.ball_x = self.WIDTH // 2 - self.BALL_RADIUS
        self.ball_y = self.paddle_y - self.BALL_RADIUS * 2
        
        # Vitesse initiale de la balle
        self.ball_speed_x = 5
        self.ball_speed_y = -5
        
        # Initialiser les briques
        self.init_bricks()
    
    def init_bricks(self):
        # Créer les briques pour le niveau actuel
        self.bricks = []
        
        # Calculer l'espace disponible et l'espacement
        available_width = self.WIDTH - 2 * self.BRICK_PADDING
        brick_width_with_padding = self.BRICK_WIDTH + self.BRICK_PADDING
        
        # Calculer le nombre de colonnes qui tiennent dans l'espace disponible
        cols = min(self.BRICK_COLS, available_width // brick_width_with_padding)
        
        # Calculer le décalage pour centrer les briques
        offset_x = (self.WIDTH - (cols * brick_width_with_padding - self.BRICK_PADDING)) // 2
        
        # Ajuster la vitesse de la balle en fonction du niveau
        base_speed = 5
        level_speed_increase = min(self.level * 0.5, 3)  # Augmentation max de 3
        self.ball_speed_x = (base_speed + level_speed_increase) * (1 if random.random() > 0.5 else -1)
        self.ball_speed_y = -(base_speed + level_speed_increase)
        
        # Ajuster la taille de la raquette en fonction du niveau (plus petite aux niveaux supérieurs)
        if self.level > 1:
            self.PADDLE_WIDTH = max(60, 100 - (self.level - 1) * 5)  # Minimum 60 pixels
        else:
            self.PADDLE_WIDTH = 100
        
        # Choisir le motif de briques en fonction du niveau
        pattern = (self.level - 1) % 5  # 5 motifs différents qui se répètent
        
        if pattern == 0:  # Motif rectangulaire classique (niveaux 1, 6, 11...)
            self._create_rectangle_pattern(offset_x, cols, brick_width_with_padding)
        elif pattern == 1:  # Motif triangulaire (niveaux 2, 7, 12...)
            self._create_triangle_pattern(offset_x, cols, brick_width_with_padding)
        elif pattern == 2:  # Motif en zigzag (niveaux 3, 8, 13...)
            self._create_zigzag_pattern(offset_x, cols, brick_width_with_padding)
        elif pattern == 3:  # Motif en croix (niveaux 4, 9, 14...)
            self._create_cross_pattern(offset_x, cols, brick_width_with_padding)
        elif pattern == 4:  # Motif aléatoire (niveaux 5, 10, 15...)
            self._create_random_pattern(offset_x, cols, brick_width_with_padding)
    
    def _create_rectangle_pattern(self, offset_x, cols, brick_width_with_padding):
        # Motif rectangulaire classique
        rows = min(self.BRICK_ROWS, 5 + (self.level - 1) // 5)  # Plus de rangées aux niveaux supérieurs
        
        for row in range(rows):
            for col in range(cols):
                # Position de la brique
                brick_x = offset_x + col * brick_width_with_padding
                brick_y = 50 + row * (self.BRICK_HEIGHT + self.BRICK_PADDING)
                
                # Propriétés de la brique
                points = (rows - row) * 10  # Les briques du haut valent plus de points
                hits_to_break = 1
                
                # Aux niveaux supérieurs, certaines briques sont plus résistantes
                if self.level > 1 and row < 2:
                    hits_to_break = min(self.level // 2 + 1, 3)  # Maximum 3 coups
                
                # Ajouter la brique à la liste
                self.bricks.append({
                    'x': brick_x,
                    'y': brick_y,
                    'points': points,
                    'hits': hits_to_break,
                    'max_hits': hits_to_break,
                    'row': row
                })
    
    def _create_triangle_pattern(self, offset_x, cols, brick_width_with_padding):
        # Motif triangulaire
        max_rows = min(self.BRICK_ROWS + 2, 7 + (self.level - 1) // 5)  # Plus de rangées aux niveaux supérieurs
        
        for row in range(max_rows):
            # Calculer le nombre de briques pour cette rangée (forme triangulaire)
            row_bricks = min(cols, row + 1)
            # Calculer le décalage pour centrer les briques de cette rangée
            row_offset = (cols - row_bricks) // 2
            
            for col in range(row_bricks):
                # Position de la brique
                brick_x = offset_x + (row_offset + col) * brick_width_with_padding
                brick_y = 50 + row * (self.BRICK_HEIGHT + self.BRICK_PADDING)
                
                # Propriétés de la brique
                points = (max_rows - row) * 10
                hits_to_break = 1 + (row % 3 == 0 and self.level > 2)  # Rangées 0, 3, 6... plus résistantes
                
                # Ajouter la brique à la liste
                self.bricks.append({
                    'x': brick_x,
                    'y': brick_y,
                    'points': points,
                    'hits': hits_to_break,
                    'max_hits': hits_to_break,
                    'row': row
                })
    
    def _create_zigzag_pattern(self, offset_x, cols, brick_width_with_padding):
        # Motif en zigzag
        rows = min(self.BRICK_ROWS + 1, 6 + (self.level - 1) // 5)
        
        for row in range(rows):
            for col in range(cols):
                # Sauter certaines briques pour créer un motif en zigzag
                if (row % 2 == 0 and col % 2 == 1) or (row % 2 == 1 and col % 2 == 0):
                    continue
                
                # Position de la brique
                brick_x = offset_x + col * brick_width_with_padding
                brick_y = 50 + row * (self.BRICK_HEIGHT + self.BRICK_PADDING)
                
                # Propriétés de la brique
                points = (rows - row) * 15  # Plus de points dans ce niveau
                hits_to_break = 1 + (col % 3 == 0 and self.level > 3)  # Colonnes 0, 3, 6... plus résistantes
                
                # Ajouter la brique à la liste
                self.bricks.append({
                    'x': brick_x,
                    'y': brick_y,
                    'points': points,
                    'hits': hits_to_break,
                    'max_hits': hits_to_break,
                    'row': row
                })
    
    def _create_cross_pattern(self, offset_x, cols, brick_width_with_padding):
        # Motif en croix
        rows = min(self.BRICK_ROWS + 1, 6 + (self.level - 1) // 5)
        center_row = rows // 2
        center_col = cols // 2
        
        for row in range(rows):
            for col in range(cols):
                # Créer une croix (garder seulement la ligne centrale et la colonne centrale)
                if row != center_row and col != center_col:
                    continue
                
                # Position de la brique
                brick_x = offset_x + col * brick_width_with_padding
                brick_y = 50 + row * (self.BRICK_HEIGHT + self.BRICK_PADDING)
                
                # Propriétés de la brique
                points = 20 + abs(center_row - row) * 10 + abs(center_col - col) * 10  # Plus de points pour les briques éloignées du centre
                hits_to_break = 1 + (row == center_row and col == center_col)  # Brique centrale plus résistante
                
                if self.level > 4:
                    hits_to_break += 1  # Toutes les briques plus résistantes aux niveaux supérieurs
                
                # Ajouter la brique à la liste
                self.bricks.append({
                    'x': brick_x,
                    'y': brick_y,
                    'points': points,
                    'hits': hits_to_break,
                    'max_hits': hits_to_break,
                    'row': row
                })
        
        # Ajouter quelques briques supplémentaires pour rendre le niveau plus difficile
        if self.level > 3:
            for row in range(1, rows-1):
                for col in range(1, cols-1):
                    if (row == center_row - 1 and col == center_col - 1) or \
                       (row == center_row - 1 and col == center_col + 1) or \
                       (row == center_row + 1 and col == center_col - 1) or \
                       (row == center_row + 1 and col == center_col + 1):
                        # Position de la brique
                        brick_x = offset_x + col * brick_width_with_padding
                        brick_y = 50 + row * (self.BRICK_HEIGHT + self.BRICK_PADDING)
                        
                        # Ajouter la brique à la liste
                        self.bricks.append({
                            'x': brick_x,
                            'y': brick_y,
                            'points': 30,
                            'hits': 1,
                            'max_hits': 1,
                            'row': row
                        })
    
    def _create_random_pattern(self, offset_x, cols, brick_width_with_padding):
        # Motif aléatoire
        rows = min(self.BRICK_ROWS + 2, 7 + (self.level - 1) // 5)
        
        # Déterminer la densité des briques (plus élevée aux niveaux supérieurs)
        density = min(0.4 + (self.level - 1) * 0.05, 0.8)  # Entre 40% et 80%
        
        for row in range(rows):
            for col in range(cols):
                # Décider aléatoirement si on place une brique ici
                if random.random() > density:
                    continue
                
                # Position de la brique
                brick_x = offset_x + col * brick_width_with_padding
                brick_y = 50 + row * (self.BRICK_HEIGHT + self.BRICK_PADDING)
                
                # Propriétés de la brique
                points = random.randint(10, 30)  # Points aléatoires
                
                # Résistance aléatoire, plus probable d'être élevée aux niveaux supérieurs
                hits_to_break = 1
                if random.random() < 0.1 * self.level:
                    hits_to_break = random.randint(2, min(3, 1 + self.level // 2))
                
                # Ajouter la brique à la liste
                self.bricks.append({
                    'x': brick_x,
                    'y': brick_y,
                    'points': points,
                    'hits': hits_to_break,
                    'max_hits': hits_to_break,
                    'row': row
                })
    
    def start_game(self):
        # Démarrer ou redémarrer le jeu
        self.game_started = True
        self.game_over = False
        
        # Changer l'apparence des boutons
        self.start_button.pack_forget()
        self.restart_button.pack(side=tk.LEFT, padx=20)
        
        # Réinitialiser le jeu
        self.reset_game()
        
        # Démarrer la boucle de jeu
        self.update()
    
    def reset_game(self):
        # Réinitialiser les variables du jeu
        self.score = 0
        self.lives = 3
        self.level = 1
        self.score_label.config(text=f"Score: {self.score}")
        self.lives_label.config(text=f"Vies: {self.lives}")
        self.level_label.config(text=f"Niveau: {self.level}")
        self.game_over = False
        
        # Effacer le canvas
        self.canvas.delete("all")
        
        # Réinitialiser la position de la raquette
        self.paddle_x = self.WIDTH // 2 - self.PADDLE_WIDTH // 2
        self.paddle_y = self.HEIGHT - 50
        
        # Réinitialiser la position de la balle
        self.ball_x = self.WIDTH // 2 - self.BALL_RADIUS
        self.ball_y = self.paddle_y - self.BALL_RADIUS * 2
        
        # Réinitialiser la vitesse de la balle
        self.ball_speed_x = 5 * (1 if random.random() > 0.5 else -1)
        self.ball_speed_y = -5
        
        # Initialiser les briques
        self.init_bricks()
        
        # Dessiner les éléments
        self.draw_elements()
    
    def key_press(self, event):
        # Gérer les touches du clavier
        key = event.keysym
        
        # Touche espace pour lancer la balle si elle est sur la raquette
        if key == "space" and self.game_started and not self.ball_moving:
            self.ball_moving = True
            self.ball_speed_x = 5 * (1 if random.random() > 0.5 else -1)
            self.ball_speed_y = -5
    
    def move_paddle(self, event):
        # Déplacer la raquette avec la souris
        if self.game_started and not self.game_over:
            # Limiter la position de la raquette aux bords du canvas
            new_x = max(0, min(event.x - self.PADDLE_WIDTH // 2, self.WIDTH - self.PADDLE_WIDTH))
            self.paddle_x = new_x
            
            # Si la balle est sur la raquette, la déplacer aussi
            if not getattr(self, 'ball_moving', True):
                self.ball_x = self.paddle_x + self.PADDLE_WIDTH // 2 - self.BALL_RADIUS
            
            # Redessiner les éléments
            self.draw_elements()
    
    def __init__(self, root):
        self.root = root
        self.root.title("Jeu de Casse-Briques")
        self.root.resizable(False, False)
        
        # Constantes du jeu
        self.WIDTH = 800
        self.HEIGHT = 600
        self.PADDLE_WIDTH = 100
        self.PADDLE_HEIGHT = 20
        self.BALL_RADIUS = 10
        self.BRICK_WIDTH = 75
        self.BRICK_HEIGHT = 30
        self.BRICK_ROWS = 5
        self.BRICK_COLS = 10
        self.BRICK_PADDING = 5
        self.GAME_SPEED = 16  # Millisecondes entre chaque mise à jour (environ 60 FPS)
        self.POWERUP_SIZE = 20  # Taille des bonus/malus
        self.POWERUP_SPEED = 3  # Vitesse de chute des bonus/malus
        self.POWERUP_DURATION_BONUS = 20000  # Durée des bonus en millisecondes (20 secondes)
        self.POWERUP_DURATION_MALUS = 10000  # Durée des malus en millisecondes (10 secondes)
        
        # Variables du jeu
        self.score = 0
        self.lives = 3
        self.level = 1
        self.game_over = False
        self.game_started = False
        self.ball_speed_x = 5
        self.ball_speed_y = -5
        self.ball_moving = False
        self.powerups = []  # Liste des bonus/malus actifs
        self.active_effects = {}  # Effets actifs avec leur temps d'expiration
        
        # Création du canvas principal
        self.canvas = tk.Canvas(root, width=self.WIDTH, height=self.HEIGHT, bg="black")
        self.canvas.pack()
        
        # Création du cadre pour les contrôles
        self.control_frame = tk.Frame(root)
        self.control_frame.pack(pady=10)
        
        # Affichage du score et des vies
        self.score_label = tk.Label(self.control_frame, text="Score: 0", font=("Arial", 14))
        self.score_label.pack(side=tk.LEFT, padx=20)
        
        self.lives_label = tk.Label(self.control_frame, text="Vies: 3", font=("Arial", 14))
        self.lives_label.pack(side=tk.LEFT, padx=20)
        
        self.level_label = tk.Label(self.control_frame, text="Niveau: 1", font=("Arial", 14))
        self.level_label.pack(side=tk.LEFT, padx=20)
        
        # Bouton pour démarrer/redémarrer le jeu
        self.start_button = tk.Button(self.control_frame, text="Commencer", 
                                     font=("Arial", 12), command=self.start_game)
        self.start_button.pack(side=tk.LEFT, padx=20)
        
        # Bouton pour redémarrer le jeu (initialement caché)
        self.restart_button = tk.Button(self.control_frame, text="Nouvelle Partie", 
                                      font=("Arial", 12), command=self.reset_game)
        self.restart_button.pack(side=tk.LEFT, padx=20)
        self.restart_button.pack_forget()  # Cacher le bouton au démarrage
        
        # Chargement des sprites
        self.load_sprites()
        
        # Initialisation des éléments du jeu
        self.init_game()
        
        # Liaison des touches du clavier et de la souris
        self.root.bind("<KeyPress>", self.key_press)
        self.root.bind("<Motion>", self.move_paddle)
        self.root.bind("<Button-1>", self.mouse_click)  # Ajouter la liaison du clic de souris
        
        # Afficher l'écran d'accueil
        self.show_welcome_screen()
    
    def mouse_click(self, event):
        # Lancer la balle avec un clic de souris si elle est sur la raquette
        if self.game_started and not self.game_over and not self.ball_moving:
            self.ball_moving = True
            self.ball_speed_x = 5 * (1 if random.random() > 0.5 else -1)
            self.ball_speed_y = -5
    
    def draw_elements(self):
        # Effacer le canvas
        self.canvas.delete("all")
        
        # Dessiner la raquette
        self.canvas.create_image(self.paddle_x, self.paddle_y, 
                               image=self.paddle_sprite, anchor="nw")
        
        # Dessiner la balle
        self.canvas.create_image(self.ball_x, self.ball_y, 
                               image=self.ball_sprite, anchor="nw")
        
        # Dessiner les briques
        for brick in self.bricks:
            # Choisir le sprite en fonction de la rangée
            sprite_index = brick['row'] % len(self.brick_sprites)
            
            # Si la brique a été touchée mais pas détruite, la rendre plus transparente
            if brick['hits'] < brick['max_hits']:
                # Créer une version plus claire de la brique pour indiquer les dégâts
                # Dans une version plus avancée, on pourrait créer des sprites spécifiques
                self.canvas.create_rectangle(
                    brick['x'], brick['y'],
                    brick['x'] + self.BRICK_WIDTH, brick['y'] + self.BRICK_HEIGHT,
                    fill="#AAAAAA", outline="#888888", width=1
                )
            else:
                self.canvas.create_image(brick['x'], brick['y'], 
                                       image=self.brick_sprites[sprite_index], anchor="nw")
        
        # Dessiner les bonus/malus
        for powerup in self.powerups:
            self.canvas.create_image(powerup['x'], powerup['y'], 
                                   image=self.powerup_sprites[powerup['type']], anchor="nw")
        
        # Afficher les effets actifs (petit indicateur visuel)
        y_pos = 10
        for effect, expiry in self.active_effects.items():
            # Calculer le temps restant
            remaining = (expiry - self.root.tk.call('clock', 'milliseconds')) / 1000
            if remaining > 0:
                # Choisir la couleur en fonction du type d'effet
                color = "#00FF00" if effect in ["paddle_grow", "ball_slow", "extra_life"] else "#FF0000"
                self.canvas.create_text(10, y_pos, text=f"{effect}: {remaining:.1f}s", 
                                      fill=color, anchor="nw", font=("Arial", 10))
                y_pos += 20
    
    def update(self):
        if self.game_started and not self.game_over:
            # Déplacer la balle
            if self.ball_moving:
                self.move_ball()
            
            # Déplacer et vérifier les bonus/malus
            self.move_powerups()
            self.check_powerup_expiry()
            
            # Redessiner les éléments
            self.draw_elements()
            
            # Planifier la prochaine mise à jour
            self.root.after(self.GAME_SPEED, self.update)
    
    def move_ball(self):
        # Déplacer la balle
        self.ball_x += self.ball_speed_x
        self.ball_y += self.ball_speed_y
        
        # Vérifier les collisions avec les bords
        # Bord gauche ou droit
        if self.ball_x <= 0 or self.ball_x + self.BALL_RADIUS * 2 >= self.WIDTH:
            self.ball_speed_x = -self.ball_speed_x
            # Ajuster la position pour éviter de rester coincé
            if self.ball_x <= 0:
                self.ball_x = 0
            else:
                self.ball_x = self.WIDTH - self.BALL_RADIUS * 2
        
        # Bord supérieur
        if self.ball_y <= 0:
            self.ball_speed_y = -self.ball_speed_y
            self.ball_y = 0
        
        # Bord inférieur (perte d'une vie)
        if self.ball_y + self.BALL_RADIUS * 2 >= self.HEIGHT:
            self.lives -= 1
            self.lives_label.config(text=f"Vies: {self.lives}")
            
            if self.lives <= 0:
                self.game_over = True
                messagebox.showinfo("Game Over", f"Partie terminée!\nScore final: {self.score}")
                return
            
            # Réinitialiser la position de la balle
            self.ball_x = self.paddle_x + self.PADDLE_WIDTH // 2 - self.BALL_RADIUS
            self.ball_y = self.paddle_y - self.BALL_RADIUS * 2
            self.ball_speed_x = 5 * (1 if random.random() > 0.5 else -1)
            self.ball_speed_y = -5
            self.ball_moving = False
        
        # Vérifier la collision avec la raquette avec une marge de sécurité
        if (self.ball_y + self.BALL_RADIUS * 2 >= self.paddle_y - 5 and 
            self.ball_y <= self.paddle_y + self.PADDLE_HEIGHT + 5 and 
            self.ball_x + self.BALL_RADIUS * 2 >= self.paddle_x - 5 and 
            self.ball_x <= self.paddle_x + self.PADDLE_WIDTH + 5):
            
            # Calculer où la balle a frappé la raquette (de -1 à 1, du bord gauche au bord droit)
            hit_position = (self.ball_x + self.BALL_RADIUS - self.paddle_x) / self.PADDLE_WIDTH - 0.5
            
            # Ajuster l'angle de rebond en fonction de l'endroit où la balle a frappé
            self.ball_speed_x = hit_position * 10
            self.ball_speed_y = -abs(self.ball_speed_y)  # Toujours rebondir vers le haut
            
            # Augmenter légèrement la vitesse
            speed_multiplier = 1.05
            self.ball_speed_x *= speed_multiplier
            self.ball_speed_y *= speed_multiplier
            
            # Limiter la vitesse maximale
            max_speed = 10
            self.ball_speed_x = max(-max_speed, min(max_speed, self.ball_speed_x))
            self.ball_speed_y = max(-max_speed, min(max_speed, self.ball_speed_y))
            
            # Ajuster la position pour éviter de rester coincé
            self.ball_y = self.paddle_y - self.BALL_RADIUS * 2
        
        # Vérifier les collisions avec les bonus/malus
        for i, powerup in enumerate(self.powerups[:]):
            if (powerup['y'] + self.POWERUP_SIZE >= self.paddle_y and 
                powerup['y'] <= self.paddle_y + self.PADDLE_HEIGHT and 
                powerup['x'] + self.POWERUP_SIZE >= self.paddle_x and 
                powerup['x'] <= self.paddle_x + self.PADDLE_WIDTH):
                
                # Appliquer l'effet du bonus/malus
                self.apply_powerup(powerup['type'])
                
                # Retirer le bonus/malus
                self.powerups.pop(i)
                break
        
        # Vérifier les collisions avec les briques
        for i, brick in enumerate(self.bricks[:]):
            if (self.ball_y + self.BALL_RADIUS * 2 >= brick['y'] and 
                self.ball_y <= brick['y'] + self.BRICK_HEIGHT and 
                self.ball_x + self.BALL_RADIUS * 2 >= brick['x'] and 
                self.ball_x <= brick['x'] + self.BRICK_WIDTH):
                
                # Déterminer de quel côté la collision s'est produite
                # Calculer les distances entre le centre de la balle et les bords de la brique
                ball_center_x = self.ball_x + self.BALL_RADIUS
                ball_center_y = self.ball_y + self.BALL_RADIUS
                
                dist_left = abs(ball_center_x - brick['x'])
                dist_right = abs(ball_center_x - (brick['x'] + self.BRICK_WIDTH))
                dist_top = abs(ball_center_y - brick['y'])
                dist_bottom = abs(ball_center_y - (brick['y'] + self.BRICK_HEIGHT))
                
                # Trouver la distance minimale pour déterminer le côté de collision
                min_dist = min(dist_left, dist_right, dist_top, dist_bottom)
                
                # Rebondir en fonction du côté de collision
                if min_dist == dist_left or min_dist == dist_right:
                    self.ball_speed_x = -self.ball_speed_x
                else:  # dist_top ou dist_bottom
                    self.ball_speed_y = -self.ball_speed_y
                
                # Réduire le nombre de coups nécessaires pour casser la brique
                brick['hits'] -= 1
                
                # Si la brique est cassée, l'enlever et ajouter des points
                if brick['hits'] <= 0:
                    self.score += brick['points']
                    self.score_label.config(text=f"Score: {self.score}")
                    # Faire apparaître un bonus/malus aléatoirement
                    self.spawn_powerup(brick['x'], brick['y'])
                    self.bricks.remove(brick)
                
                # Ne vérifier qu'une seule collision de brique par frame
                break
        
        # Vérifier si toutes les briques sont détruites
        if not self.bricks:
            self.level += 1
            self.level_label.config(text=f"Niveau: {self.level}")
            
            # Bonus de points pour avoir terminé le niveau
            level_bonus = self.level * 50
            self.score += level_bonus
            self.score_label.config(text=f"Score: {self.score}")
            
            # Afficher un message
            messagebox.showinfo("Niveau terminé", 
                              f"Niveau {self.level-1} terminé!\nBonus: +{level_bonus} points\n\nPréparez-vous pour le niveau {self.level}")
            
            # Initialiser le niveau suivant
            self.init_bricks()
            
            # Réinitialiser la position de la balle
            self.ball_x = self.paddle_x + self.PADDLE_WIDTH // 2 - self.BALL_RADIUS
            self.ball_y = self.paddle_y - self.BALL_RADIUS * 2
            
            # Augmenter la vitesse de la balle pour le niveau suivant
            base_speed = 5 + self.level
            self.ball_speed_x = base_speed * (1 if random.random() > 0.5 else -1)
            self.ball_speed_y = -base_speed
            self.ball_moving = False
            
            # Réinitialiser les bonus/malus
            self.powerups = []
            self.active_effects = {}
    
    def update_powerups(self):
        # Déplacer les bonus/malus vers le bas
        for i, powerup in enumerate(self.powerups[:]):
            powerup['y'] += self.POWERUP_SPEED
            
            # Supprimer les bonus/malus qui sortent de l'écran
            if powerup['y'] > self.HEIGHT:
                self.powerups.pop(i)
    
    def spawn_powerup(self, x, y):
        # Chance de 20% de faire apparaître un bonus/malus
        if random.random() < 0.2:
            # Liste des bonus et malus disponibles
            powerups = [
                # Bonus (40% de chance)
                {'type': 'paddle_grow', 'good': True},
                {'type': 'ball_slow', 'good': True},
                {'type': 'extra_life', 'good': True},
                # Malus (60% de chance)
                {'type': 'paddle_shrink', 'good': False},
                {'type': 'ball_fast', 'good': False}
            ]
            
            # Sélectionner un bonus/malus
            powerup = random.choice(powerups)
            
            # Créer le bonus/malus
            self.powerups.append({
                'x': x + self.BRICK_WIDTH // 2 - self.POWERUP_SIZE // 2,
                'y': y + self.BRICK_HEIGHT,
                'type': powerup['type'],
                'good': powerup['good']
            })
    
    def move_powerups(self):
        # Déplacer tous les bonus/malus actifs
        for powerup in self.powerups[:]:
            powerup['y'] += self.POWERUP_SPEED
            
            # Vérifier la collision avec la raquette
            if (powerup['y'] + self.POWERUP_SIZE >= self.paddle_y and
                powerup['y'] <= self.paddle_y + self.PADDLE_HEIGHT and
                powerup['x'] + self.POWERUP_SIZE >= self.paddle_x and
                powerup['x'] <= self.paddle_x + self.PADDLE_WIDTH):
                
                # Appliquer l'effet
                self.apply_powerup_effect(powerup['type'])
                self.powerups.remove(powerup)
            
            # Supprimer si sorti de l'écran
            elif powerup['y'] > self.HEIGHT:
                self.powerups.remove(powerup)
    
    def apply_powerup_effect(self, effect_type):
        current_time = self.root.tk.call('clock', 'milliseconds')
        # Différencier la durée entre bonus et malus
        if effect_type in ['paddle_grow', 'ball_slow', 'extra_life']:
            expiry_time = current_time + self.POWERUP_DURATION_BONUS
        else:
            expiry_time = current_time + self.POWERUP_DURATION_MALUS
        
        # Appliquer l'effet
        if effect_type == 'paddle_grow':
            # Sauvegarder la taille originale avant modification
            original_width = self.PADDLE_WIDTH
            # Doubler la taille de la raquette et créer un nouveau sprite
            self.PADDLE_WIDTH = min(250, self.PADDLE_WIDTH * 2)  # Augmenter de 100%
            try:
                paddle_img = Image.new("RGBA", (int(self.PADDLE_WIDTH), int(self.PADDLE_HEIGHT)), (0, 0, 0, 0))
                paddle_draw = ImageDraw.Draw(paddle_img)
                for y in range(int(self.PADDLE_HEIGHT)):
                    blue_val = 200 - int(y * 100 / self.PADDLE_HEIGHT)
                    paddle_draw.line([(0, y), (int(self.PADDLE_WIDTH), y)], fill=(0, 100, blue_val))
                paddle_draw.rectangle([0, 0, int(self.PADDLE_WIDTH), int(self.PADDLE_HEIGHT)], outline="#4040FF", width=2)
                self.paddle_sprite = ImageTk.PhotoImage(paddle_img)
            except Exception as e:
                # En cas d'erreur, restaurer la taille originale
                self.PADDLE_WIDTH = original_width
                print(f"Erreur lors de la création du sprite: {e}")
        elif effect_type == 'paddle_shrink':
            # Sauvegarder la taille originale avant modification
            original_width = self.PADDLE_WIDTH
            self.PADDLE_WIDTH = max(40, self.PADDLE_WIDTH * 0.75)  # Réduire de 25%
            try:
                paddle_img = Image.new("RGBA", (int(self.PADDLE_WIDTH), int(self.PADDLE_HEIGHT)), (0, 0, 0, 0))
                paddle_draw = ImageDraw.Draw(paddle_img)
                for y in range(int(self.PADDLE_HEIGHT)):
                    blue_val = 200 - int(y * 100 / self.PADDLE_HEIGHT)
                    paddle_draw.line([(0, y), (int(self.PADDLE_WIDTH), y)], fill=(0, 100, blue_val))
                paddle_draw.rectangle([0, 0, int(self.PADDLE_WIDTH), int(self.PADDLE_HEIGHT)], outline="#4040FF", width=2)
                self.paddle_sprite = ImageTk.PhotoImage(paddle_img)
            except Exception as e:
                # En cas d'erreur, restaurer la taille originale
                self.PADDLE_WIDTH = original_width
                print(f"Erreur lors de la création du sprite: {e}")
        elif effect_type == 'ball_slow':
            self.ball_speed_x *= 0.5  # Ralentissement plus important
            self.ball_speed_y *= 0.5
        elif effect_type == 'ball_fast':
            self.ball_speed_x *= 2.0
            self.ball_speed_y *= 2.0
        elif effect_type == 'extra_life':
            self.lives += 1
            self.lives_label.config(text=f"Vies: {self.lives}")
            return  # Pas besoin d'expiration pour la vie supplémentaire
        
        # Enregistrer l'effet et son temps d'expiration
        self.active_effects[effect_type] = expiry_time
    
    def check_powerup_expiry(self):
        current_time = self.root.tk.call('clock', 'milliseconds')
        
        # Vérifier chaque effet actif
        for effect_type, expiry_time in list(self.active_effects.items()):
            if current_time >= expiry_time:
                # Annuler l'effet
                if effect_type in ['paddle_grow', 'paddle_shrink']:
                    self.PADDLE_WIDTH = 100  # Taille normale
                    # Recréer le sprite de la raquette avec la taille normale
                    paddle_img = Image.new("RGBA", (self.PADDLE_WIDTH, self.PADDLE_HEIGHT), (0, 0, 0, 0))
                    paddle_draw = ImageDraw.Draw(paddle_img)
                    for y in range(self.PADDLE_HEIGHT):
                        blue_val = 200 - int(y * 100 / self.PADDLE_HEIGHT)
                        paddle_draw.line([(0, y), (self.PADDLE_WIDTH, y)], fill=(0, 100, blue_val))
                    paddle_draw.rectangle([0, 0, self.PADDLE_WIDTH, self.PADDLE_HEIGHT], outline="#4040FF", width=2)
                    self.paddle_sprite = ImageTk.PhotoImage(paddle_img)
                elif effect_type == 'ball_slow':
                    self.ball_speed_x *= 1.33  # Retour à la vitesse normale
                    self.ball_speed_y *= 1.33
                elif effect_type == 'ball_fast':
                    self.ball_speed_x *= 0.67  # Retour à la vitesse normale
                    self.ball_speed_y *= 0.67
                
                # Supprimer l'effet expiré
                del self.active_effects[effect_type]

    def apply_powerup(self, powerup_type):
        """Applique l'effet d'un bonus/malus"""
        # Enregistrer le temps d'activation
        activation_time = self.root.after(0, lambda: None)  # Temps actuel en ms
        
        if powerup_type == "paddle_grow":
            # Agrandir la raquette
            self.PADDLE_WIDTH = min(150, self.PADDLE_WIDTH + 30)
            self.active_effects["paddle_grow"] = activation_time + self.POWERUP_DURATION_BONUS
            
        elif powerup_type == "ball_slow":
            # Ralentir la balle
            self.ball_speed_x *= 0.7
            self.ball_speed_y *= 0.7
            self.active_effects["ball_slow"] = activation_time + self.POWERUP_DURATION_BONUS
            
        elif powerup_type == "extra_life":
            # Vie supplémentaire
            self.lives += 1
            self.lives_label.config(text=f"Vies: {self.lives}")
            
        elif powerup_type == "paddle_shrink":
            # Rétrécir la raquette
            self.PADDLE_WIDTH = max(50, self.PADDLE_WIDTH - 30)
            self.active_effects["paddle_shrink"] = activation_time + self.POWERUP_DURATION_MALUS
            
        elif powerup_type == "ball_fast":
            # Accélérer la balle
            self.ball_speed_x *= 1.3
            self.ball_speed_y *= 1.3
            self.active_effects["ball_fast"] = activation_time + self.POWERUP_DURATION_MALUS

if __name__ == "__main__":
    root = tk.Tk()
    game = BrickGame(root)
    root.mainloop()