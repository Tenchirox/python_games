import tkinter as tk
from tkinter import Frame, Label, Button, Canvas, Scrollbar
from PIL import Image, ImageTk, ImageDraw
import os
import sys
import importlib.util

class GameMenu:
    def __init__(self, root):
        self.root = root
        self.root.title("Game Selection Menu")
        self.root.geometry("800x600")
        self.root.configure(bg="#2F2F2F")
        
        # Title
        title_label = Label(root, text="Select a Game", font=("Arial", 24, "bold"), 
                           bg="#2F2F2F", fg="white")
        title_label.pack(pady=20)
        
        # Create a canvas with scrollbar for the games
        self.canvas_frame = Frame(root, bg="#2F2F2F")
        self.canvas_frame.pack(fill="both", expand=True, padx=20, pady=10)
        
        self.canvas = Canvas(self.canvas_frame, bg="#2F2F2F", highlightthickness=0)
        self.scrollbar = Scrollbar(self.canvas_frame, orient="vertical", command=self.canvas.yview)
        
        self.canvas.configure(yscrollcommand=self.scrollbar.set)
        self.scrollbar.pack(side="right", fill="y")
        self.canvas.pack(side="left", fill="both", expand=True)
        
        # Games container inside canvas
        self.games_frame = Frame(self.canvas, bg="#2F2F2F")
        self.canvas_window = self.canvas.create_window((0, 0), window=self.games_frame, anchor="nw")
        
        # Bind to window resize event
        self.root.bind("<Configure>", self.on_window_configure)
        
        # Bind mousewheel events for scrolling
        self.canvas.bind("<MouseWheel>", self.on_mousewheel)  # Windows
        self.canvas.bind("<Button-4>", self.on_mousewheel)    # Linux scroll up
        self.canvas.bind("<Button-5>", self.on_mousewheel)    # Linux scroll down
        
        # Available games
        self.games = [
            {
                "name": "Pac-Man",
                "description": "Classic arcade game where you eat dots and avoid ghosts",
                "module": "pacman2",
                "class": "PacManGame",
                "thumbnail": self.create_pacman_thumbnail()
            },
            {
                "name": "Snake",
                "description": "Guide the snake to eat food and grow without hitting walls or itself",
                "module": "snake",
                "class": "SnakeGame",
                "thumbnail": self.create_snake_thumbnail()
            },
            {
                "name": "Tetris",
                "description": "Arrange falling blocks to create complete lines",
                "module": "tetris",
                "class": "TetrisGame",
                "thumbnail": self.create_tetris_thumbnail()
            },
            {
                "name": "Space Invaders",
                "description": "Defend Earth from waves of alien invaders",
                "module": "space_invaders",
                "class": "SpaceInvadersGame",
                "thumbnail": self.create_space_invaders_thumbnail()
            },
            {
                "name": "Connect Four",
                "description": "Drop discs to connect four of your color in a row",
                "module": "puissance4",
                "class": "Puissance4Game",
                "thumbnail": self.create_connect_four_thumbnail()
            },
            {
                "name": "Brick Breaker",
                "description": "Break all the bricks with a bouncing ball",
                "module": "casse_briques",
                "class": "BrickGame",
                "thumbnail": self.create_brick_breaker_thumbnail()
            },
            {
                "name": "Solitaire",
                "description": "Classic card game of patience and strategy",
                "module": "solitaire",
                "class": "SolitaireGame",
                "thumbnail": self.create_solitaire_thumbnail()
            },
            {
                "name": "Micro Machines",
                "description": "Race tiny cars around various tracks",
                "module": "micromachines",
                "class": "Car",  # This might need adjustment based on the actual class name
                "thumbnail": self.create_micromachines_thumbnail()
            }
        ]
        
        # Create game cards
        self.create_game_cards()
    
    def create_pacman_thumbnail(self):
        # Create a thumbnail for Pac-Man
        size = 150
        img = Image.new("RGBA", (size, size), (0, 0, 0, 255))
        draw = ImageDraw.Draw(img)
        
        # Draw maze-like background
        cell_size = 15
        for y in range(0, size, cell_size):
            for x in range(0, size, cell_size):
                # Create a maze-like pattern
                if (x // cell_size + y // cell_size) % 2 == 0:
                    draw.rectangle([x, y, x + cell_size - 1, y + cell_size - 1], fill="#0000AA")
        
        # Draw Pac-Man
        pacman_color = "#FFFF00"  # Yellow
        pacman_size = 40
        pacman_x = size // 2
        pacman_y = size // 2
        
        # Draw Pac-Man body (circle with mouth)
        draw.ellipse([pacman_x - pacman_size//2, pacman_y - pacman_size//2, 
                     pacman_x + pacman_size//2, pacman_y + pacman_size//2], 
                    fill=pacman_color)
        
        # Draw mouth
        draw.polygon([pacman_x, pacman_y, 
                     pacman_x + pacman_size//2, pacman_y - pacman_size//4,
                     pacman_x + pacman_size//2, pacman_y + pacman_size//4], 
                    fill="black")
        
        # Draw ghosts
        ghost_colors = ["#FF0000", "#00FFFF", "#FFB8FF", "#FFB852"]  # Red, Cyan, Pink, Orange
        ghost_size = 25
        
        for i, color in enumerate(ghost_colors):
            ghost_x = 30 + i * 30
            ghost_y = size - 40
            
            # Ghost body
            draw.rectangle([ghost_x - ghost_size//2, ghost_y - ghost_size//2 + ghost_size//4, 
                          ghost_x + ghost_size//2, ghost_y + ghost_size//2], 
                         fill=color)
            draw.ellipse([ghost_x - ghost_size//2, ghost_y - ghost_size//2, 
                         ghost_x + ghost_size//2, ghost_y + ghost_size//2 - ghost_size//4], 
                        fill=color)
            
            # Eyes
            eye_size = ghost_size // 5
            draw.ellipse([ghost_x - ghost_size//4 - eye_size//2, ghost_y - ghost_size//4 - eye_size//2, 
                         ghost_x - ghost_size//4 + eye_size//2, ghost_y - ghost_size//4 + eye_size//2], 
                        fill="white")
            draw.ellipse([ghost_x + ghost_size//4 - eye_size//2, ghost_y - ghost_size//4 - eye_size//2, 
                         ghost_x + ghost_size//4 + eye_size//2, ghost_y - ghost_size//4 + eye_size//2], 
                        fill="white")
        
        # Draw some dots
        dot_size = 5
        for i in range(5):
            dot_x = 30 + i * 25
            dot_y = 30
            draw.ellipse([dot_x - dot_size//2, dot_y - dot_size//2, 
                         dot_x + dot_size//2, dot_y + dot_size//2], 
                        fill="white")
        
        # Draw title
        draw.text((size//2, 15), "PAC-MAN", fill="yellow", anchor="mt")
        
        return ImageTk.PhotoImage(img)
    
    def create_snake_thumbnail(self):
        # Create a thumbnail for Snake
        size = 150
        img = Image.new("RGBA", (size, size), (0, 0, 0, 255))
        draw = ImageDraw.Draw(img)
        
        # Draw grass background
        draw.rectangle([0, 0, size, size], fill="#003300")
        
        # Draw snake
        snake_color = "#90EE90"  # Light green
        head_color = "#006400"   # Dark green
        segment_size = 15
        
        # Snake body segments
        segments = [(size//2 - i*segment_size, size//2) for i in range(8)]
        
        # Draw body segments
        for segment in segments[1:]:
            x, y = segment
            draw.ellipse([x-segment_size//2, y-segment_size//2, 
                         x+segment_size//2, y+segment_size//2], 
                        fill=snake_color)
        
        # Draw head
        head_x, head_y = segments[0]
        draw.ellipse([head_x-segment_size//2, head_y-segment_size//2, 
                     head_x+segment_size//2, head_y+segment_size//2], 
                    fill=head_color)
        
        # Draw eyes
        eye_size = segment_size // 5
        draw.ellipse([head_x-segment_size//4-eye_size//2, head_y-eye_size//2, 
                     head_x-segment_size//4+eye_size//2, head_y+eye_size//2], 
                    fill="white")
        draw.ellipse([head_x+segment_size//4-eye_size//2, head_y-eye_size//2, 
                     head_x+segment_size//4+eye_size//2, head_y+eye_size//2], 
                    fill="white")
        
        # Draw food (apple)
        food_x, food_y = size//2 + 40, size//2
        food_size = segment_size
        draw.ellipse([food_x-food_size//2, food_y-food_size//2, 
                     food_x+food_size//2, food_y+food_size//2], 
                    fill="#FF0000")
        
        # Draw title
        draw.text((size//2, 15), "SNAKE", fill="white", anchor="mt")
        
        return ImageTk.PhotoImage(img)
    
    def create_tetris_thumbnail(self):
        # Create a thumbnail for Tetris
        size = 150
        img = Image.new("RGBA", (size, size), (0, 0, 0, 255))
        draw = ImageDraw.Draw(img)
        
        # Draw background
        draw.rectangle([0, 0, size, size], fill="#000000")
        
        # Draw tetris pieces
        block_size = 15
        tetris_colors = ["#00FFFF", "#0000FF", "#FF7F00", "#FFFF00", "#00FF00", "#800080", "#FF0000"]
        
        # I piece (cyan)
        for i in range(4):
            draw.rectangle([20, 20 + i*block_size, 20 + block_size, 20 + (i+1)*block_size], 
                         fill=tetris_colors[0], outline="#FFFFFF", width=1)
        
        # L piece (orange)
        for i in range(3):
            draw.rectangle([40, 20 + i*block_size, 40 + block_size, 20 + (i+1)*block_size], 
                         fill=tetris_colors[2], outline="#FFFFFF", width=1)
        draw.rectangle([40 + block_size, 20 + 2*block_size, 40 + 2*block_size, 20 + 3*block_size], 
                     fill=tetris_colors[2], outline="#FFFFFF", width=1)
        
        # Square piece (yellow)
        for i in range(2):
            for j in range(2):
                draw.rectangle([60 + i*block_size, 20 + j*block_size, 60 + (i+1)*block_size, 20 + (j+1)*block_size], 
                             fill=tetris_colors[3], outline="#FFFFFF", width=1)
        
        # T piece (purple)
        draw.rectangle([80, 20, 80 + block_size, 20 + block_size], 
                     fill=tetris_colors[5], outline="#FFFFFF", width=1)
        for i in range(3):
            draw.rectangle([70 + i*block_size, 20 + block_size, 70 + (i+1)*block_size, 20 + 2*block_size], 
                         fill=tetris_colors[5], outline="#FFFFFF", width=1)
        
        # Draw some falling blocks
        for i in range(5):
            x = 30 + i * 20
            y = 80 + (i % 3) * 15
            color = tetris_colors[i % len(tetris_colors)]
            draw.rectangle([x, y, x + block_size, y + block_size], 
                         fill=color, outline="#FFFFFF", width=1)
        
        # Draw title
        draw.text((size//2, 15), "TETRIS", fill="white", anchor="mt")
        
        return ImageTk.PhotoImage(img)
    
    def create_space_invaders_thumbnail(self):
        # Create a thumbnail for Space Invaders
        size = 150
        img = Image.new("RGBA", (size, size), (0, 0, 0, 255))
        draw = ImageDraw.Draw(img)
        
        # Draw space background
        draw.rectangle([0, 0, size, size], fill="#000000")
        
        # Draw some stars
        for _ in range(20):
            import random
            x = random.randint(5, size-5)
            y = random.randint(5, size-5)
            star_size = random.randint(1, 3)
            draw.ellipse([x-star_size, y-star_size, x+star_size, y+star_size], fill="#FFFFFF")
        
        # Draw aliens
        alien_colors = ["#FF0000", "#00FFFF", "#FFFF00"]
        alien_size = 15
        
        for row in range(3):
            for col in range(5):
                x = 20 + col * (alien_size + 5)
                y = 30 + row * (alien_size + 5)
                color = alien_colors[row % len(alien_colors)]
                
                # Draw alien body
                draw.ellipse([x, y, x + alien_size, y + alien_size], fill=color)
                # Draw alien eyes
                eye_size = alien_size // 5
                draw.ellipse([x + alien_size//3 - eye_size//2, y + alien_size//3 - eye_size//2, 
                             x + alien_size//3 + eye_size//2, y + alien_size//3 + eye_size//2], 
                            fill="#000000")
                draw.ellipse([x + 2*alien_size//3 - eye_size//2, y + alien_size//3 - eye_size//2, 
                             x + 2*alien_size//3 + eye_size//2, y + alien_size//3 + eye_size//2], 
                            fill="#000000")
        
        # Draw player ship
        ship_width = 20
        ship_height = 10
        ship_x = size // 2 - ship_width // 2
        ship_y = size - 30
        
        draw.polygon([ship_x, ship_y + ship_height, 
                     ship_x + ship_width//2, ship_y, 
                     ship_x + ship_width, ship_y + ship_height], 
                    fill="#00FF00")
        
        # Draw bullets
        for i in range(2):
            bullet_x = size // 2
            bullet_y = ship_y - 20 - i * 20
            draw.rectangle([bullet_x - 1, bullet_y - 5, bullet_x + 1, bullet_y], fill="#FFFFFF")
        
        # Draw title
        draw.text((size//2, 15), "SPACE INVADERS", fill="white", anchor="mt")
        
        return ImageTk.PhotoImage(img)
    
    def create_connect_four_thumbnail(self):
        # Create a thumbnail for Connect Four
        size = 150
        img = Image.new("RGBA", (size, size), (0, 0, 0, 255))
        draw = ImageDraw.Draw(img)
        
        # Draw background
        draw.rectangle([0, 0, size, size], fill="#0000AA")
        
        # Draw the board
        board_width = 120
        board_height = 100
        board_x = (size - board_width) // 2
        board_y = (size - board_height) // 2 + 10
        
        draw.rectangle([board_x, board_y, board_x + board_width, board_y + board_height], 
                     fill="#0000AA", outline="#FFFFFF", width=2)
        
        # Draw the slots
        slot_size = 15
        for row in range(6):
            for col in range(7):
                x = board_x + 10 + col * (slot_size + 2)
                y = board_y + 10 + row * (slot_size + 2)
                draw.ellipse([x, y, x + slot_size, y + slot_size], fill="#FFFFFF")
        
        # Draw some pieces
        pieces = [
            (1, 5, "#FF0000"), (2, 5, "#FFFF00"), (3, 5, "#FF0000"),
            (2, 4, "#FF0000"), (3, 4, "#FFFF00"), (4, 5, "#FFFF00"),
            (4, 4, "#FF0000")
        ]
        
        for col, row, color in pieces:
            x = board_x + 10 + col * (slot_size + 2)
            y = board_y + 10 + row * (slot_size + 2)
            draw.ellipse([x, y, x + slot_size, y + slot_size], fill=color)
        
        # Draw title
        draw.text((size//2, 15), "CONNECT FOUR", fill="white", anchor="mt")
        
        return ImageTk.PhotoImage(img)
    
    def create_brick_breaker_thumbnail(self):
        # Create a thumbnail for Brick Breaker
        size = 150
        img = Image.new("RGBA", (size, size), (0, 0, 0, 255))
        draw = ImageDraw.Draw(img)
        
        # Draw background
        draw.rectangle([0, 0, size, size], fill="#000000")
        
        # Draw bricks
        brick_colors = ["#FF0000", "#FF7F00", "#FFFF00", "#00FF00", "#0000FF", "#4B0082", "#9400D3"]
        brick_width = 20
        brick_height = 10
        
        for row in range(5):
            for col in range(6):
                x = 10 + col * (brick_width + 2)
                y = 30 + row * (brick_height + 2)
                color = brick_colors[row % len(brick_colors)]
                draw.rectangle([x, y, x + brick_width, y + brick_height], 
                             fill=color, outline="#FFFFFF", width=1)
        
        # Draw paddle
        paddle_width = 40
        paddle_height = 10
        paddle_x = size // 2 - paddle_width // 2
        paddle_y = size - 30
        
        draw.rectangle([paddle_x, paddle_y, paddle_x + paddle_width, paddle_y + paddle_height], 
                     fill="#4040FF", outline="#FFFFFF", width=1)
        
        # Draw ball
        ball_size = 8
        ball_x = size // 2
        ball_y = size // 2 + 20
        
        draw.ellipse([ball_x - ball_size//2, ball_y - ball_size//2, 
                     ball_x + ball_size//2, ball_y + ball_size//2], 
                    fill="#FFFFFF")
        
        # Draw title
        draw.text((size//2, 15), "BRICK BREAKER", fill="white", anchor="mt")
        
        return ImageTk.PhotoImage(img)
    
    def create_solitaire_thumbnail(self):
        # Create a thumbnail for Solitaire
        size = 150
        img = Image.new("RGBA", (size, size), (0, 0, 0, 255))
        draw = ImageDraw.Draw(img)
        
        # Draw green background like a card table
        draw.rectangle([0, 0, size, size], fill="#2F4F4F")
        
        # Draw some cards
        card_width = 25
        card_height = 35
        
        # Draw stacked cards in tableau
        for i in range(7):
            x = 10 + i * 18
            for j in range(min(i+1, 3)):
                y = 50 + j * 15
                # Card back or face
                if j < i:
                    # Card back
                    draw.rectangle([x, y, x + card_width, y + card_height], 
                                 fill="#000080", outline="#FFFFFF", width=1)
                else:
                    # Card face
                    draw.rectangle([x, y, x + card_width, y + card_height], 
                                 fill="#FFFFFF", outline="#000000", width=1)
                    # Card value
                    suit_colors = {"♥": "#FF0000", "♦": "#FF0000", "♣": "#000000", "♠": "#000000"}
                    suit = list(suit_colors.keys())[i % 4]
                    draw.text((x + 3, y + 3), f"{(i+j) % 13 + 1}", fill=suit_colors[suit])
                    draw.text((x + 3, y + 15), suit, fill=suit_colors[suit])
        
        # Draw foundation piles
        for i in range(4):
            x = 80 + i * 15
            y = 20
            draw.rectangle([x, y, x + card_width, y + card_height], 
                         fill="#FFFFFF", outline="#000000", width=1)
            if i > 0:  # Add some cards to foundations
                suit = list(suit_colors.keys())[i-1]
                draw.text((x + 3, y + 3), "A", fill=suit_colors[suit])
                draw.text((x + 3, y + 15), suit, fill=suit_colors[suit])
        
        # Draw title
        draw.text((size//2, 15), "SOLITAIRE", fill="white", anchor="mt")
        
        return ImageTk.PhotoImage(img)
    
    def create_micromachines_thumbnail(self):
        # Create a thumbnail for MicroMachines
        size = 150
        img = Image.new("RGBA", (size, size), (0, 0, 0, 255))
        draw = ImageDraw.Draw(img)
        
        # Draw race track background
        draw.rectangle([0, 0, size, size], fill="#00AA00")  # Green grass
        
        # Draw race track
        track_width = 30
        draw.ellipse([20, 20, size-20, size-20], fill="#00AA00", outline="#AAAAAA", width=track_width)
        
        # Draw start/finish line
        draw.rectangle([size//2-5, size//2-track_width//2, size//2+5, size//2+track_width//2], fill="#FFFFFF")
        
        # Draw cars
        car_colors = ["#FF0000", "#0000FF", "#FFFF00", "#00FFFF"]
        car_width = 15
        car_height = 8
        
        for i, color in enumerate(car_colors):
            # Calculate position on track
            angle = i * 90 + 45  # Degrees
            import math
            radius = size//2 - track_width//2 - 5
            x = size//2 + int(radius * math.cos(math.radians(angle)))
            y = size//2 + int(radius * math.sin(math.radians(angle)))
            
            # Draw car body
            draw.rectangle([x-car_width//2, y-car_height//2, x+car_width//2, y+car_height//2], 
                         fill=color, outline="#000000", width=1)
            
            # Draw car details
            draw.rectangle([x+car_width//2-3, y-car_height//2+1, x+car_width//2-1, y+car_height//2-1], 
                         fill="#FFFFFF")
        
        # Draw title
        draw.text((size//2, 15), "MICRO MACHINES", fill="white", anchor="mt")
        
        return ImageTk.PhotoImage(img)
    
    def create_game_cards(self):
        # Create a card for each game
        games_per_row = 3  # Nombre de jeux par ligne
        
        for i, game in enumerate(self.games):
            # Create a frame for the game
            game_frame = Frame(self.games_frame, bg="#3F3F3F", padx=10, pady=10, 
                              relief="raised", borderwidth=2)
            game_frame.grid(row=i//games_per_row, column=i%games_per_row, padx=20, pady=20, sticky="nsew")
            
            # Game thumbnail
            thumbnail_label = Label(game_frame, image=game["thumbnail"], bg="#3F3F3F")
            thumbnail_label.pack(pady=10)
            
            # Game name
            name_label = Label(game_frame, text=game["name"], font=("Arial", 14, "bold"), 
                              bg="#3F3F3F", fg="white")
            name_label.pack()
            
            # Game description
            desc_label = Label(game_frame, text=game["description"], font=("Arial", 10), 
                              bg="#3F3F3F", fg="#CCCCCC", wraplength=200)
            desc_label.pack(pady=5)
            
            # Play button
            play_button = Button(game_frame, text="Play", font=("Arial", 12, "bold"), 
                                bg="#4CAF50", fg="white", padx=20, pady=5,
                                command=lambda g=game: self.launch_game(g))
            play_button.pack(pady=10)
            
        # Configure grid weights to make rows and columns expandable
        for i in range((len(self.games) + games_per_row - 1) // games_per_row):  # Rows
            self.games_frame.grid_rowconfigure(i, weight=1)
        
        for i in range(games_per_row):  # Columns
            self.games_frame.grid_columnconfigure(i, weight=1)
        
        # Update the scrollregion after all game cards are created
        self.update_scrollregion()
        
        # Bind to configure event to update scrollregion when window is resized
        self.games_frame.bind("<Configure>", self.on_frame_configure)
    
    def update_scrollregion(self):
        # Update the scrollregion to encompass all the game cards
        self.games_frame.update_idletasks()
        self.canvas.config(scrollregion=self.canvas.bbox("all"))
    
    def on_frame_configure(self, event=None):
        # Reset the scroll region to encompass the inner frame
        self.canvas.configure(scrollregion=self.canvas.bbox("all"))
        
        # Make sure the window is wide enough for the frame
        frame_width = self.games_frame.winfo_reqwidth()
        if frame_width > self.canvas.winfo_width():
            self.canvas.config(width=frame_width)
    
    def on_window_configure(self, event=None):
        # Update canvas size when window is resized
        if event.widget == self.root:
            # Only process if the root window is being resized
            canvas_width = self.canvas_frame.winfo_width() - self.scrollbar.winfo_width() - 4
            canvas_height = self.canvas_frame.winfo_height() - 4
            
            if canvas_width > 0 and canvas_height > 0:
                self.canvas.config(width=canvas_width, height=canvas_height)
                
            # Update the scrollregion
            self.update_scrollregion()
    
    def on_mousewheel(self, event):
        # Handle mousewheel scrolling
        if event.num == 5 or event.delta < 0:
            # Scroll down (Windows uses delta, Linux uses num)
            self.canvas.yview_scroll(1, "units")
        elif event.num == 4 or event.delta > 0:
            # Scroll up
            self.canvas.yview_scroll(-1, "units")
    
    def launch_game(self, game):
        # Get the module path
        module_name = game["module"]
        module_path = os.path.join(os.path.dirname(__file__), f"{module_name}.py")
        
        if not os.path.exists(module_path):
            print(f"Error: Game module {module_path} not found.")
            return
        
        # Import the module
        spec = importlib.util.spec_from_file_location(module_name, module_path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        
        # Close the menu window
        self.root.destroy()
        
        # Create a new root window for the game
        game_root = tk.Tk()
        game_root.title(game["name"])
        
        # Launch the game with the new root window
        game_class = getattr(module, game["class"])
        
        # Try to initialize the game with root parameter, if it fails, try without
        try:
            game_instance = game_class(game_root)
        except TypeError:
            try:
                game_instance = game_class()
            except Exception as e:
                print(f"Error launching game: {e}")
                game_root.destroy()
                return
        
        # If the game has a run method, call it
        if hasattr(game_instance, "run"):
            game_instance.run()
        
        # Start the game's main loop if it doesn't have a run method
        else:
            game_root.mainloop()

if __name__ == "__main__":
    root = tk.Tk()
    app = GameMenu(root)
    root.mainloop()