import tkinter as tk
from tkinter import messagebox
import random
import os
from PIL import Image, ImageTk, ImageDraw

class TetrisGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Tetris")
        self.root.resizable(False, False)
        
        # Constantes du jeu
        self.WIDTH = 400
        self.HEIGHT = 600
        self.GRID_SIZE = 30
        self.GRID_WIDTH = 10
        self.GRID_HEIGHT = 20
        self.GAME_SPEED = 500  # Millisecondes entre chaque descente automatique
        self.SPEED_INCREASE = 50  # Réduction du délai à chaque niveau
        self.MIN_SPEED = 100  # Vitesse maximale
        
        # Variables du jeu
        self.score = 0
        self.level = 1
        self.lines_cleared = 0
        self.game_over = False
        self.game_started = False
        self.paused = False
        
        # Création du canvas principal
        self.canvas = tk.Canvas(root, width=self.WIDTH, height=self.HEIGHT, bg="black")
        self.canvas.pack(side=tk.LEFT)
        
        # Panneau d'information
        self.info_frame = tk.Frame(root, width=200, height=self.HEIGHT, bg="#2F2F2F")
        self.info_frame.pack(side=tk.RIGHT, fill=tk.BOTH)
        
        # Création du cadre pour les contrôles
        self.control_frame = tk.Frame(self.info_frame, bg="#2F2F2F")
        self.control_frame.pack(pady=10)
        
        # Affichage du score et du niveau
        self.score_label = tk.Label(self.info_frame, text="Score: 0", font=("Arial", 14), bg="#2F2F2F", fg="white")
        self.score_label.pack(pady=10)
        
        self.level_label = tk.Label(self.info_frame, text="Niveau: 1", font=("Arial", 14), bg="#2F2F2F", fg="white")
        self.level_label.pack(pady=10)
        
        self.lines_label = tk.Label(self.info_frame, text="Lignes: 0", font=("Arial", 14), bg="#2F2F2F", fg="white")
        self.lines_label.pack(pady=10)
        
        # Aperçu de la pièce suivante
        self.next_piece_label = tk.Label(self.info_frame, text="Pièce suivante:", font=("Arial", 14), bg="#2F2F2F", fg="white")
        self.next_piece_label.pack(pady=(30, 10))
        
        self.next_piece_canvas = tk.Canvas(self.info_frame, width=120, height=120, bg="black")
        self.next_piece_canvas.pack(pady=10)
        
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
        
        # Couleurs des pièces de Tetris
        self.colors = {
            "I": "#00FFFF",  # Cyan
            "J": "#0000FF",  # Bleu
            "L": "#FF7F00",  # Orange
            "O": "#FFFF00",  # Jaune
            "S": "#00FF00",  # Vert
            "T": "#800080",  # Violet
            "Z": "#FF0000"   # Rouge
        }
        
        # Créer les sprites pour chaque type de bloc
        self.block_sprites = {}
        for piece_type, color in self.colors.items():
            self.block_sprites[piece_type] = self.create_block_sprite(color)
    
    def create_block_sprite(self, color):
        # Créer un sprite pour un bloc
        block_size = self.GRID_SIZE - 2  # Légèrement plus petit pour voir la grille
        img = Image.new("RGBA", (block_size, block_size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Dessiner le bloc avec un effet 3D
        draw.rectangle([0, 0, block_size-1, block_size-1], fill=color)
        
        # Bord supérieur et gauche plus clair (effet de lumière)
        lighter_color = self.lighten_color(color)
        draw.line([(0, 0), (block_size-1, 0)], fill=lighter_color, width=2)  # Haut
        draw.line([(0, 0), (0, block_size-1)], fill=lighter_color, width=2)  # Gauche
        
        # Bord inférieur et droit plus foncé (effet d'ombre)
        darker_color = self.darken_color(color)
        draw.line([(0, block_size-1), (block_size-1, block_size-1)], fill=darker_color, width=2)  # Bas
        draw.line([(block_size-1, 0), (block_size-1, block_size-1)], fill=darker_color, width=2)  # Droite
        
        return ImageTk.PhotoImage(img)
    
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
                              text="TETRIS", 
                              font=("Arial", 36, "bold"), 
                              fill="white")
        
        # Instructions
        instructions = [
            "Utilisez les flèches pour déplacer les pièces",
            "Flèche haut pour faire pivoter",
            "Flèche bas pour accélérer la descente",
            "Espace pour faire tomber instantanément",
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
        
        # Dessiner quelques blocs décoratifs
        for piece_type, color in self.colors.items():
            x = random.randint(0, self.GRID_WIDTH - 1) * self.GRID_SIZE
            y = random.randint(10, self.GRID_HEIGHT - 1) * self.GRID_SIZE
            self.canvas.create_image(x, y, image=self.block_sprites[piece_type], anchor="nw")
    
    def init_game(self):
        # Initialiser les variables du jeu
        self.score = 0
        self.level = 1
        self.lines_cleared = 0
        self.game_over = False
        self.paused = False
        
        # Initialiser la grille de jeu (0 = vide, sinon contient le type de pièce)
        self.grid = [[0 for _ in range(self.GRID_WIDTH)] for _ in range(self.GRID_HEIGHT)]
        
        # Définir les formes des pièces
        self.pieces = {
            "I": [[(0, 0), (0, 1), (0, 2), (0, 3)],
                  [(0, 0), (1, 0), (2, 0), (3, 0)]],
            "J": [[(0, 0), (1, 0), (1, 1), (1, 2)],
                  [(0, 0), (0, 1), (1, 0), (2, 0)],
                  [(0, 0), (0, 1), (0, 2), (1, 2)],
                  [(0, 1), (1, 1), (2, 0), (2, 1)]],
            "L": [[(0, 0), (0, 1), (0, 2), (1, 0)],
                  [(0, 0), (1, 0), (2, 0), (2, 1)],
                  [(0, 2), (1, 0), (1, 1), (1, 2)],
                  [(0, 0), (0, 1), (1, 1), (2, 1)]],
            "O": [[(0, 0), (0, 1), (1, 0), (1, 1)]],
            "S": [[(0, 1), (0, 2), (1, 0), (1, 1)],
                  [(0, 0), (1, 0), (1, 1), (2, 1)]],
            "T": [[(0, 1), (1, 0), (1, 1), (1, 2)],
                  [(0, 0), (1, 0), (1, 1), (2, 0)],
                  [(0, 0), (0, 1), (0, 2), (1, 1)],
                  [(0, 1), (1, 0), (1, 1), (2, 1)]],
            "Z": [[(0, 0), (0, 1), (1, 1), (1, 2)],
                  [(0, 1), (1, 0), (1, 1), (2, 0)]]
        }
        
        # Générer la pièce actuelle et la suivante
        self.generate_new_piece()
        self.generate_next_piece()
    
    def setup_bindings(self):
        # Configurer les touches de contrôle
        self.root.bind("<Left>", self.move_left)
        self.root.bind("<Right>", self.move_right)
        self.root.bind("<Down>", self.move_down)
        self.root.bind("<Up>", self.rotate)
        self.root.bind("<space>", self.drop)
        self.root.bind("p", self.toggle_pause)
        self.root.bind("P", self.toggle_pause)
    
    def start_game(self):
        # Démarrer le jeu
        self.game_started = True
        self.game_over = False
        
        # Changer l'apparence des boutons
        self.start_button.pack_forget()
        self.restart_button.pack(pady=10)
        
        # Réinitialiser le jeu
        self.reset_game()
        
        # Démarrer la boucle de jeu
        self.game_loop()
    
    def reset_game(self):
        # Réinitialiser le jeu
        self.init_game()
        self.score_label.config(text=f"Score: {self.score}")
        self.level_label.config(text=f"Niveau: {self.level}")
        self.lines_label.config(text=f"Lignes: {self.lines_cleared}")
        self.draw_grid()
        self.draw_current_piece()
        self.draw_next_piece()
    
    def generate_new_piece(self):
        # Générer une nouvelle pièce
        if hasattr(self, 'next_piece_type'):
            self.current_piece_type = self.next_piece_type
            self.current_rotation = 0
        else:
            self.current_piece_type = random.choice(list(self.pieces.keys()))
            self.current_rotation = 0
        
        # Position initiale (centrée en haut)
        self.current_x = self.GRID_WIDTH // 2 - 1
        self.current_y = 0
        
        # Vérifier si la nouvelle pièce peut être placée
        if not self.is_valid_position():
            self.game_over = True
    
    def generate_next_piece(self):
        # Générer la pièce suivante
        self.next_piece_type = random.choice(list(self.pieces.keys()))
        self.draw_next_piece()
    
    def get_current_piece_shape(self):
        # Obtenir la forme actuelle de la pièce en fonction de sa rotation
        return self.pieces[self.current_piece_type][self.current_rotation % len(self.pieces[self.current_piece_type])]
    
    def is_valid_position(self, x=None, y=None, rotation=None):
        # Vérifier si la position est valide (pas de collision avec les bords ou d'autres pièces)
        if x is None:
            x = self.current_x
        if y is None:
            y = self.current_y
        if rotation is None:
            rotation = self.current_rotation
        
        # Obtenir la forme de la pièce avec la rotation spécifiée
        shape = self.pieces[self.current_piece_type][rotation % len(self.pieces[self.current_piece_type])]
        
        for block_x, block_y in shape:
            # Calculer la position absolue du bloc
            grid_x = x + block_x
            grid_y = y + block_y
            
            # Vérifier si le bloc est dans les limites de la grille
            if grid_x < 0 or grid_x >= self.GRID_WIDTH or grid_y < 0 or grid_y >= self.GRID_HEIGHT:
                return False
            
            # Vérifier s'il y a déjà un bloc à cette position
            if grid_y >= 0 and self.grid[grid_y][grid_x] != 0:
                return False
        
        return True
    
    def move_left(self, event=None):
        # Déplacer la pièce vers la gauche
        if self.game_started and not self.game_over and not self.paused:
            if self.is_valid_position(x=self.current_x - 1):
                self.current_x -= 1
                self.draw_grid()
                self.draw_current_piece()
    
    def move_right(self, event=None):
        # Déplacer la pièce vers la droite
        if self.game_started and not self.game_over and not self.paused:
            if self.is_valid_position(x=self.current_x + 1):
                self.current_x += 1
                self.draw_grid()
                self.draw_current_piece()
    
    def move_down(self, event=None):
        # Déplacer la pièce vers le bas
        if self.game_started and not self.game_over and not self.paused:
            if self.is_valid_position(y=self.current_y + 1):
                self.current_y += 1
                self.draw_grid()
                self.draw_current_piece()
            else:
                self.lock_piece()
    
    def rotate(self, event=None):
        # Faire pivoter la pièce
        if self.game_started and not self.game_over and not self.paused:
            new_rotation = (self.current_rotation + 1) % len(self.pieces[self.current_piece_type])
            if self.is_valid_position(rotation=new_rotation):
                self.current_rotation = new_rotation
                self.draw_grid()
                self.draw_current_piece()
    
    def drop(self, event=None):
        # Faire tomber la pièce instantanément
        if self.game_started and not self.game_over and not self.paused:
            while self.is_valid_position(y=self.current_y + 1):
                self.current_y += 1
            self.lock_piece()
    
    def toggle_pause(self, event=None):
        # Mettre en pause ou reprendre le jeu
        if self.game_started and not self.game_over:
            self.paused = not self.paused
            if self.paused:
                self.canvas.create_text(self.WIDTH // 2, self.HEIGHT // 2, 
                                      text="PAUSE", 
                                      font=("Arial", 36, "bold"), 
                                      fill="white", 
                                      tags="pause")
            else:
                self.canvas.delete("pause")
                self.game_loop()
    
    def lock_piece(self):
        # Verrouiller la pièce en place et vérifier les lignes complètes
        shape = self.get_current_piece_shape()
        
        for block_x, block_y in shape:
            grid_x = self.current_x + block_x
            grid_y = self.current_y + block_y
            
            if 0 <= grid_y < self.GRID_HEIGHT and 0 <= grid_x < self.GRID_WIDTH:
                self.grid[grid_y][grid_x] = self.current_piece_type
        
        # Vérifier si une brique touche le haut de l'écran (première ligne)
        if any(self.grid[0][x] != 0 for x in range(self.GRID_WIDTH)):
            self.game_over = True
            self.show_game_over()
            return
        
        # Vérifier et supprimer les lignes complètes
        self.check_lines()
        
        # Générer une nouvelle pièce
        self.generate_new_piece()
        self.generate_next_piece()
        
        # Mettre à jour l'affichage
        self.draw_grid()
        self.draw_current_piece()
    
    def check_lines(self):
        # Vérifier les lignes complètes et les supprimer
        lines_to_clear = []
        
        for y in range(self.GRID_HEIGHT):
            if all(self.grid[y][x] != 0 for x in range(self.GRID_WIDTH)):
                lines_to_clear.append(y)
        
        if not lines_to_clear:
            return
        
        # Mettre à jour le score et le niveau
        lines_count = len(lines_to_clear)
        self.lines_cleared += lines_count
        
        # Système de score classique de Tetris
        points_per_line = {1: 100, 2: 300, 3: 500, 4: 800}
        self.score += points_per_line.get(lines_count, 100 * lines_count) * self.level
        
        # Mettre à jour le niveau (tous les 10 lignes)
        self.level = self.lines_cleared // 10 + 1
        
        # Mettre à jour les labels
        self.score_label.config(text=f"Score: {self.score}")
        self.level_label.config(text=f"Niveau: {self.level}")
        self.lines_label.config(text=f"Lignes: {self.lines_cleared}")
        
        # Animation de suppression des lignes
        self.animate_line_clear(lines_to_clear)
        
        # Supprimer les lignes et faire descendre les blocs
        for line in sorted(lines_to_clear):
            del self.grid[line]
            self.grid.insert(0, [0 for _ in range(self.GRID_WIDTH)])
    
    def animate_line_clear(self, lines):
        # Animation simple pour la suppression des lignes
        for line in lines:
            for x in range(self.GRID_WIDTH):
                self.canvas.create_rectangle(
                    x * self.GRID_SIZE, line * self.GRID_SIZE,
                    (x + 1) * self.GRID_SIZE, (line + 1) * self.GRID_SIZE,
                    fill="white", outline="white"
                )
        
        self.canvas.update()
        self.root.after(100)  # Pause pour l'effet visuel
    
    def draw_grid(self):
        # Dessiner la grille et les blocs verrouillés
        self.canvas.delete("all")
        
        # Dessiner les lignes de la grille
        for x in range(0, self.GRID_WIDTH * self.GRID_SIZE, self.GRID_SIZE):
            self.canvas.create_line(x, 0, x, self.GRID_HEIGHT * self.GRID_SIZE, fill="#333333")
        
        for y in range(0, self.GRID_HEIGHT * self.GRID_SIZE, self.GRID_SIZE):
            self.canvas.create_line(0, y, self.GRID_WIDTH * self.GRID_SIZE, y, fill="#333333")
        
        # Dessiner les blocs verrouillés
        for y in range(self.GRID_HEIGHT):
            for x in range(self.GRID_WIDTH):
                if self.grid[y][x] != 0:
                    piece_type = self.grid[y][x]
                    self.canvas.create_image(
                        x * self.GRID_SIZE, y * self.GRID_SIZE,
                        image=self.block_sprites[piece_type],
                        anchor="nw"
                    )
    
    def draw_current_piece(self):
        # Dessiner la pièce actuelle
        if self.game_over:
            return
        
        shape = self.get_current_piece_shape()
        
        for block_x, block_y in shape:
            x = (self.current_x + block_x) * self.GRID_SIZE
            y = (self.current_y + block_y) * self.GRID_SIZE
            
            self.canvas.create_image(
                x, y,
                image=self.block_sprites[self.current_piece_type],
                anchor="nw"
            )
    
    def draw_next_piece(self):
        # Dessiner la pièce suivante dans le panneau d'aperçu
        self.next_piece_canvas.delete("all")
        
        if not hasattr(self, 'next_piece_type'):
            return
        
        # Obtenir la forme de la pièce suivante (première rotation)
        shape = self.pieces[self.next_piece_type][0]
        
        # Calculer le centre pour l'affichage
        min_x = min(block_x for block_x, _ in shape)
        max_x = max(block_x for block_x, _ in shape)
        min_y = min(block_y for _, block_y in shape)
        max_y = max(block_y for _, block_y in shape)
        
        width = max_x - min_x + 1
        height = max_y - min_y + 1
        
        offset_x = (120 - width * self.GRID_SIZE) // 2
        offset_y = (120 - height * self.GRID_SIZE) // 2
        
        for block_x, block_y in shape:
            x = offset_x + (block_x - min_x) * self.GRID_SIZE
            y = offset_y + (block_y - min_y) * self.GRID_SIZE
            
            self.next_piece_canvas.create_image(
                x, y,
                image=self.block_sprites[self.next_piece_type],
                anchor="nw"
            )
    
    def game_loop(self):
        # Boucle principale du jeu
        if self.game_started and not self.game_over and not self.paused:
            # Faire descendre la pièce automatiquement
            if self.is_valid_position(y=self.current_y + 1):
                self.current_y += 1
            else:
                self.lock_piece()
                
                # Vérifier si le jeu est terminé
                if self.game_over:
                    self.show_game_over()
                    return
            
            # Mettre à jour l'affichage
            self.draw_grid()
            self.draw_current_piece()
            
            # Calculer la vitesse en fonction du niveau
            speed = max(self.MIN_SPEED, self.GAME_SPEED - (self.level - 1) * self.SPEED_INCREASE)
            
            # Planifier la prochaine mise à jour
            self.root.after(speed, self.game_loop)
    
    def show_game_over(self):
        # Afficher l'écran de fin de jeu
        self.canvas.create_rectangle(
            50, self.HEIGHT // 2 - 60,
            self.WIDTH - 50, self.HEIGHT // 2 + 60,
            fill="#000000", outline="#FFFFFF", width=2
        )
        
        self.canvas.create_text(
            self.WIDTH // 2, self.HEIGHT // 2 - 30,
            text="GAME OVER",
            font=("Arial", 24, "bold"),
            fill="white"
        )
        
        self.canvas.create_text(
            self.WIDTH // 2, self.HEIGHT // 2 + 10,
            text=f"Score final: {self.score}",
            font=("Arial", 18),
            fill="white"
        )
        
        # Afficher un message pour redémarrer
        self.canvas.create_text(
            self.WIDTH // 2, self.HEIGHT // 2 + 40,
            text="Cliquez sur 'Nouvelle Partie' pour recommencer",
            font=("Arial", 12),
            fill="white"
        )

if __name__ == "__main__":
    root = tk.Tk()
    game = TetrisGame(root)
    root.mainloop()