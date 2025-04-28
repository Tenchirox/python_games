import tkinter as tk
from tkinter import messagebox
import random
import os
from PIL import Image, ImageTk, ImageDraw

class SnakeGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Jeu de Snake")
        self.root.resizable(False, False)
        
        # Constantes du jeu
        self.WIDTH = 600
        self.HEIGHT = 600
        self.GRID_SIZE = 30
        self.GRID_WIDTH = self.WIDTH // self.GRID_SIZE
        self.GRID_HEIGHT = self.HEIGHT // self.GRID_SIZE
        self.GAME_SPEED = 150  # Millisecondes entre chaque mise à jour
        
        # Variables du jeu
        self.direction = "Right"
        self.next_direction = "Right"
        self.score = 0
        self.game_over = False
        self.game_started = False
        
        # Création du canvas principal
        self.canvas = tk.Canvas(root, width=self.WIDTH, height=self.HEIGHT, bg="black")
        self.canvas.pack()
        
        # Création du cadre pour les contrôles
        self.control_frame = tk.Frame(root)
        self.control_frame.pack(pady=10)
        
        # Affichage du score
        self.score_label = tk.Label(self.control_frame, text="Score: 0", font=("Arial", 14))
        self.score_label.pack(side=tk.LEFT, padx=20)
        
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
        
        # Initialisation du serpent et de la nourriture (sans démarrer le jeu)
        self.init_game()
        
        # Liaison des touches du clavier
        self.root.bind("<KeyPress>", self.change_direction)
        
        # Afficher l'écran d'accueil
        self.show_welcome_screen()
    
    def load_sprites(self):
        # Créer un dossier pour les sprites s'il n'existe pas
        sprites_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sprites")
        if not os.path.exists(sprites_dir):
            os.makedirs(sprites_dir)
        
        # Créer des sprites plus élaborés par défaut
        # Tête du serpent (carré vert avec des yeux)
        head_size = self.GRID_SIZE
        head_img = Image.new("RGBA", (head_size, head_size), (0, 0, 0, 0))
        head_draw = ImageDraw.Draw(head_img)
        
        # Dessiner la tête (cercle vert foncé)
        head_draw.ellipse([2, 2, head_size-2, head_size-2], fill="#006400")
        
        # Dessiner les yeux (deux petits cercles blancs avec pupilles noires)
        eye_size = head_size // 6
        # Œil gauche
        head_draw.ellipse([head_size//3-eye_size, head_size//3-eye_size, 
                          head_size//3+eye_size, head_size//3+eye_size], fill="white")
        head_draw.ellipse([head_size//3-eye_size//2, head_size//3-eye_size//2, 
                          head_size//3+eye_size//2, head_size//3+eye_size//2], fill="black")
        # Œil droit
        head_draw.ellipse([2*head_size//3-eye_size, head_size//3-eye_size, 
                          2*head_size//3+eye_size, head_size//3+eye_size], fill="white")
        head_draw.ellipse([2*head_size//3-eye_size//2, head_size//3-eye_size//2, 
                          2*head_size//3+eye_size//2, head_size//3+eye_size//2], fill="black")
        
        self.head_sprite = ImageTk.PhotoImage(head_img)
        
        # Corps du serpent (cercle vert clair)
        body_img = Image.new("RGBA", (head_size, head_size), (0, 0, 0, 0))
        body_draw = ImageDraw.Draw(body_img)
        body_draw.ellipse([2, 2, head_size-2, head_size-2], fill="#90EE90")
        self.body_sprite = ImageTk.PhotoImage(body_img)
        
        # Nourriture (pomme rouge)
        food_img = Image.new("RGBA", (head_size, head_size), (0, 0, 0, 0))
        food_draw = ImageDraw.Draw(food_img)
        
        # Dessiner la pomme (cercle rouge)
        food_draw.ellipse([2, 2, head_size-2, head_size-2], fill="#FF0000")
        
        # Dessiner la queue de la pomme (petit rectangle marron)
        stem_width = head_size // 8
        stem_height = head_size // 4
        food_draw.rectangle([head_size//2-stem_width//2, 0, 
                            head_size//2+stem_width//2, stem_height], fill="#8B4513")
        
        # Dessiner une petite feuille verte
        leaf_size = head_size // 5
        food_draw.ellipse([head_size//2, stem_height, 
                          head_size//2+leaf_size, stem_height+leaf_size], fill="#00FF00")
        
        self.food_sprite = ImageTk.PhotoImage(food_img)
    
    def create_rectangle_sprite(self, width, height, color):
        # Créer une image simple pour représenter les éléments du jeu
        image = Image.new("RGB", (width, height), color)
        return ImageTk.PhotoImage(image)
    
    def show_welcome_screen(self):
        # Afficher un écran d'accueil avec des instructions
        self.canvas.delete("all")
        
        # Titre du jeu
        self.canvas.create_text(self.WIDTH // 2, self.HEIGHT // 4, 
                              text="JEU DE SNAKE", 
                              font=("Arial", 36, "bold"), 
                              fill="white")
        
        # Instructions
        instructions = [
            "Utilisez les flèches du clavier pour diriger le serpent",
            "Mangez les pommes pour grandir et marquer des points",
            "Évitez les murs et ne vous mordez pas la queue!",
            "\nCliquez sur 'Commencer' pour jouer"
        ]
        
        y_pos = self.HEIGHT // 2
        for line in instructions:
            self.canvas.create_text(self.WIDTH // 2, y_pos, 
                                  text=line, 
                                  font=("Arial", 16), 
                                  fill="white")
            y_pos += 30
        
        # Dessiner un petit serpent décoratif
        snake_demo_x = self.WIDTH // 2
        snake_demo_y = self.HEIGHT * 3 // 4
        
        # Tête
        self.canvas.create_image(snake_demo_x, snake_demo_y, 
                               image=self.head_sprite, anchor="center")
        
        # Corps (3 segments)
        for i in range(1, 4):
            self.canvas.create_image(snake_demo_x - i * self.GRID_SIZE, snake_demo_y, 
                                   image=self.body_sprite, anchor="center")
        
        # Nourriture
        self.canvas.create_image(snake_demo_x + self.GRID_SIZE * 2, snake_demo_y, 
                               image=self.food_sprite, anchor="center")
    
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
    
    def init_game(self):
        # Initialiser le jeu sans le démarrer
        self.direction = "Right"
        self.next_direction = "Right"
        self.score = 0
        self.score_label.config(text="Score: 0")
        
        # Initialiser le serpent au centre (sans le dessiner)
        center_x = self.GRID_WIDTH // 2
        center_y = self.GRID_HEIGHT // 2
        self.snake = [
            (center_x, center_y),
            (center_x - 1, center_y),
            (center_x - 2, center_y)
        ]
    
    def reset_game(self):
        # Réinitialiser les variables du jeu
        self.direction = "Right"
        self.next_direction = "Right"
        self.score = 0
        self.score_label.config(text="Score: 0")
        self.game_over = False
        
        # Effacer le canvas
        self.canvas.delete("all")
        
        # Initialiser le serpent au centre
        center_x = self.GRID_WIDTH // 2
        center_y = self.GRID_HEIGHT // 2
        self.snake = [
            (center_x, center_y),
            (center_x - 1, center_y),
            (center_x - 2, center_y)
        ]
        
        # Dessiner le serpent initial
        self.draw_snake()
        
        # Placer la nourriture
        self.place_food()
        
        # Si le jeu n'est pas déjà démarré, ne pas lancer la boucle de jeu
        if self.game_started:
            self.root.after(self.GAME_SPEED, self.update)
    
    def draw_snake(self):
        # Dessiner chaque segment du serpent
        for i, (x, y) in enumerate(self.snake):
            # Convertir les coordonnées de la grille en pixels
            x1 = x * self.GRID_SIZE
            y1 = y * self.GRID_SIZE
            
            # Utiliser le sprite de tête pour le premier segment, sinon le sprite de corps
            if i == 0:
                self.canvas.create_image(x1, y1, image=self.head_sprite, anchor="nw", tags="snake")
            else:
                self.canvas.create_image(x1, y1, image=self.body_sprite, anchor="nw", tags="snake")
    
    def place_food(self):
        # Trouver une position libre pour la nourriture
        while True:
            x = random.randint(0, self.GRID_WIDTH - 1)
            y = random.randint(0, self.GRID_HEIGHT - 1)
            
            # Vérifier que la position n'est pas occupée par le serpent
            if (x, y) not in self.snake:
                break
        
        # Stocker la position de la nourriture
        self.food_pos = (x, y)
        
        # Dessiner la nourriture
        food_x = x * self.GRID_SIZE
        food_y = y * self.GRID_SIZE
        self.canvas.create_image(food_x, food_y, image=self.food_sprite, anchor="nw", tags="food")
    
    def change_direction(self, event):
        # Changer la direction en fonction de la touche pressée
        key = event.keysym
        
        # Empêcher les changements de direction à 180 degrés
        if key == "Up" and self.direction != "Down":
            self.next_direction = "Up"
        elif key == "Down" and self.direction != "Up":
            self.next_direction = "Down"
        elif key == "Left" and self.direction != "Right":
            self.next_direction = "Left"
        elif key == "Right" and self.direction != "Left":
            self.next_direction = "Right"
    
    def move_snake(self):
        # Mettre à jour la direction
        self.direction = self.next_direction
        
        # Obtenir la position de la tête
        head_x, head_y = self.snake[0]
        
        # Calculer la nouvelle position de la tête
        if self.direction == "Up":
            new_head = (head_x, head_y - 1)
        elif self.direction == "Down":
            new_head = (head_x, head_y + 1)
        elif self.direction == "Left":
            new_head = (head_x - 1, head_y)
        elif self.direction == "Right":
            new_head = (head_x + 1, head_y)
        
        # Vérifier les collisions avec les murs
        new_x, new_y = new_head
        if new_x < 0 or new_x >= self.GRID_WIDTH or new_y < 0 or new_y >= self.GRID_HEIGHT:
            self.game_over = True
            messagebox.showinfo("Game Over", f"Vous avez touché un mur!\nScore final: {self.score}")
            return
        
        # Vérifier les collisions avec le serpent lui-même
        if new_head in self.snake:
            self.game_over = True
            messagebox.showinfo("Game Over", f"Vous vous êtes mordu!\nScore final: {self.score}")
            return
        
        # Ajouter la nouvelle tête
        self.snake.insert(0, new_head)
        
        # Vérifier si le serpent a mangé la nourriture
        if new_head == self.food_pos:
            # Augmenter le score
            self.score += 10
            self.score_label.config(text=f"Score: {self.score}")
            
            # Placer une nouvelle nourriture
            self.canvas.delete("food")
            self.place_food()
        else:
            # Supprimer la queue si le serpent n'a pas mangé
            self.snake.pop()
        
        # Redessiner le serpent
        self.canvas.delete("snake")
        self.draw_snake()
    
    def update(self):
        # Ne mettre à jour le jeu que s'il a été démarré et n'est pas terminé
        if self.game_started and not self.game_over:
            # Déplacer le serpent
            self.move_snake()
            
            # Planifier la prochaine mise à jour
            self.root.after(self.GAME_SPEED, self.update)

if __name__ == "__main__":
    root = tk.Tk()
    game = SnakeGame(root)
    root.mainloop()