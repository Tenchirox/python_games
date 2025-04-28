import tkinter as tk
from tkinter import messagebox
import random
import os
from PIL import Image, ImageTk, ImageDraw

class SpaceInvadersGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Space Invaders")
        self.root.resizable(False, False)

        # Constantes du jeu
        self.WIDTH = 800
        self.HEIGHT = 600
        self.PLAYER_SPEED = 5  # Vitesse réduite pour un mouvement plus fluide
        self.BULLET_SPEED = 15
        self.ENEMY_SPEED = 2
        self.ENEMY_ROWS = 5
        self.ENEMY_COLS = 11
        self.GAME_SPEED = 30
        self.current_level = 1
        self.level_configs = [
            {'rows':3, 'cols':8, 'speed':2, 'color':'#FF0000'},
            {'rows':4, 'cols':10, 'speed':3, 'color':'#FFFF00'},
            {'rows':5, 'cols':11, 'speed':4, 'color':'#00FFFF'}
        ]

        # Variables du jeu
        self.score = 0
        self.lives = 3
        self.game_over = False
        self.game_started = False
        self.bullets = []
        self.enemies = []
        self.barriers = []

        # Création du canvas
        self.canvas = tk.Canvas(root, width=self.WIDTH, height=self.HEIGHT, bg="black")
        self.canvas.pack()

        # Chargement des sprites
        self.load_sprites()
        self.create_widgets()
        self.init_game()
        self.setup_bindings()

    def load_sprites(self):
        # Sprites du joueur
        player_img = Image.new("RGBA", (50, 30), (0,0,0,0))
        draw = ImageDraw.Draw(player_img)
        draw.polygon([(25,0), (45,30), (5,30)], fill="#00FF00")
        self.player_sprite = ImageTk.PhotoImage(player_img)

        # Sprites des ennemis
        self.enemy_sprites = [
            self.create_alien_sprite("#FF0000"),
            self.create_alien_sprite("#FFFF00"),
            self.create_alien_sprite("#00FFFF")
        ]

        # Sprite projectile
        bullet_img = Image.new("RGBA", (4, 10), (0,0,0,0))
        draw = ImageDraw.Draw(bullet_img)
        draw.rectangle([0,0,3,9], fill="#FFFFFF")
        self.bullet_sprite = ImageTk.PhotoImage(bullet_img)

    def create_alien_sprite(self, color):
        img = Image.new("RGBA", (40, 30), (0,0,0,0))
        draw = ImageDraw.Draw(img)
        draw.ellipse([0,0,39,29], fill=color)
        draw.ellipse([10,5,29,24], fill="#000000")
        return ImageTk.PhotoImage(img)

    def create_widgets(self):
        # Cadre de contrôle
        self.control_frame = tk.Frame(self.root)
        self.control_frame.pack(pady=10)

        # Affichage du score et des vies
        self.score_label = tk.Label(self.control_frame, text=f"Score: {self.score}", font=("Arial", 14))
        self.score_label.pack(side=tk.LEFT, padx=20)

        self.lives_label = tk.Label(self.control_frame, text=f"Vies: {self.lives}", font=("Arial", 14))
        self.lives_label.pack(side=tk.LEFT, padx=20)

        # Boutons de contrôle
        self.start_button = tk.Button(self.control_frame, text="Commencer", 
                                    font=("Arial", 12), command=self.start_game)
        self.start_button.pack(side=tk.LEFT, padx=20)

        self.restart_button = tk.Button(self.control_frame, text="Nouvelle Partie", 
                                      font=("Arial", 12), command=self.reset_game)
        self.restart_button.pack_forget()

    def draw(self):
        self.canvas.delete("all")
        # Dessin du joueur
        self.canvas.create_image(self.player_x, self.player_y, 
                               image=self.player_sprite, anchor="center")
        
        # Dessin des ennemis
        for enemy in self.enemies:
            self.canvas.create_image(enemy['x'], enemy['y'], 
                                   image=enemy['sprite'], anchor="center")
        
        # Dessin des projectiles
        for bullet in self.bullets:
            self.canvas.create_image(bullet['x'], bullet['y'],
                                   image=self.bullet_sprite, anchor="center")
        
        # Dessin des barrières
        for barrier in self.barriers:
            if barrier['health'] > 0:
                self.canvas.create_rectangle(barrier['x']-30, barrier['y']-15,
                                           barrier['x']+30, barrier['y']+15,
                                           fill="#00FF00", outline="#FFFFFF")

    def move_enemies(self):
        # Logique de déplacement des ennemis
        for enemy in self.enemies:
            enemy['x'] += self.ENEMY_SPEED
        
        # Inversion de direction si on touche les bords
        if any(enemy['x'] > self.WIDTH-50 or enemy['x'] < 50 for enemy in self.enemies):
            self.ENEMY_SPEED *= -1
            for enemy in self.enemies:
                enemy['y'] += 20

    def move_bullets(self):
        # Mise à jour position des projectiles
        self.bullets = [b for b in self.bullets if b['y'] > 0]
        for bullet in self.bullets:
            bullet['y'] -= self.BULLET_SPEED

    def check_collisions(self):
        # Détection collisions projectiles/ennemis
        for bullet in self.bullets[:]:
            for enemy in self.enemies[:]:
                if (abs(bullet['x'] - enemy['x']) < 25 and
                    abs(bullet['y'] - enemy['y']) < 25):
                    self.score += 10
                    self.bullets.remove(bullet)
                    self.enemies.remove(enemy)
                    self.score_label.config(text=f"Score: {self.score}")
                    break

        # Vérification victoire
        if not self.enemies:
            self.game_over = True
            self.current_level += 1
            if self.current_level > len(self.level_configs):
                messagebox.showinfo("Victoire!", f"Score final: {self.score}")
            else:
                self.create_enemy_wave()
                self.score_label.config(text=f"Score: {self.score}  Niveau: {self.current_level}")

    def start_game(self):
        self.game_started = True
        self.game_over = False
        self.start_button.pack_forget()
        self.restart_button.pack()
        self.game_loop()

    def reset_game(self):
        self.score = 0
        self.lives = 3
        self.enemies.clear()
        self.bullets.clear()
        self.create_enemy_wave()
        self.score_label.config(text="Score: 0")
        self.lives_label.config(text="Vies: 3")
        self.game_over = False
        self.game_loop()

    def init_game(self):
        # Initialisation des positions
        self.player_x = self.WIDTH // 2
        self.player_y = self.HEIGHT - 50
        self.create_enemy_wave()
        self.create_barriers()

    def create_enemy_wave(self):
        # Création de la vague d'ennemis
        config = self.level_configs[self.current_level-1]
        for row in range(config['rows']):
            for col in range(config['cols']):
                x = 100 + col * (self.WIDTH-200)//config['cols']
                y = 50 + row * 40
                self.enemies.append({
                    'x': x,
                    'y': y,
                    'sprite': self.create_alien_sprite(config['color'])
                })
        self.ENEMY_SPEED = config['speed']

    def create_barriers(self):
        # Création des barrières de protection
        for i in range(4):
            x = 100 + i * 200
            y = self.HEIGHT - 150
            self.barriers.append({
                'x': x,
                'y': y,
                'health': 10
            })

    def setup_bindings(self):
        self.root.bind('<KeyPress-Left>', self.start_move_left)
        self.root.bind('<KeyPress-Right>', self.start_move_right)
        self.root.bind('<KeyRelease-Left>', self.stop_move)
        self.root.bind('<KeyRelease-Right>', self.stop_move)
        self.root.bind('<space>', self.shoot)
        
        self.move_direction = 0

    def start_move_left(self, event):
        self.move_direction = -1
        self.move_continuously()

    def start_move_right(self, event):
        self.move_direction = 1
        self.move_continuously()

    def stop_move(self, event):
        self.move_direction = 0

    def move_continuously(self):
        if self.move_direction != 0 and not self.game_over:
            self.player_x = max(50, min(self.WIDTH - 50, self.player_x + self.PLAYER_SPEED * self.move_direction))
            self.root.after(20, self.move_continuously)

    def on_key_release(self, event):
        self.move_delta = 1.0  # Réinitialisation de l'accélération

    def shoot(self, event):
        if len(self.bullets) < 3:
            self.bullets.append({
                'x': self.player_x + 25,
                'y': self.player_y
            })

    def game_loop(self):
        if not self.game_over:
            self.move_enemies()
            self.move_bullets()
            self.check_collisions()
            self.draw()
        self.root.after(self.GAME_SPEED, self.game_loop)

    # Méthodes supplémentaires pour la logique du jeu...

if __name__ == "__main__":
    root = tk.Tk()
    game = SpaceInvadersGame(root)
    root.mainloop()