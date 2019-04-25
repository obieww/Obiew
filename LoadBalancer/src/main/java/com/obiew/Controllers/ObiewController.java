package com.obiew.Controllers;

import com.obiew.Entities.Obiew;
import com.obiew.Entities.User;
import com.obiew.Repositories.ObiewRepository;
import com.obiew.Repositories.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api/obiew")
public class ObiewController {
    private ObiewRepository obiewRepository;
    private UserRepository userRepository;

    @Autowired
    public ObiewController(ObiewRepository obiewRepository, UserRepository userRepository) {
        this.obiewRepository = obiewRepository;
        this.userRepository = userRepository;
    }

    @GetMapping("/{obiewId}")
    public ResponseEntity<Obiew> getById(@PathVariable String obiewId) {
        Obiew obiew = obiewRepository.findByObiewId(obiewId);
        if (obiew != null) {
            return new ResponseEntity<>(obiew, HttpStatus.OK);
        } else {
            return new ResponseEntity<>(null, HttpStatus.FORBIDDEN);
        }
    }

    @PostMapping("/post")
    public ResponseEntity<Obiew> add(@RequestBody Obiew obiew) {
        User user = userRepository.findByUserId(obiew.getUser().getUserId());
        if(user != null) {
            user.addObiew(obiew);
            obiew = obiewRepository.save(obiew);
            user = userRepository.save(user);
            return new ResponseEntity<>(obiew, HttpStatus.OK);
        } else {
            return new ResponseEntity<>(null, HttpStatus.FORBIDDEN);
        }
    }

    @DeleteMapping("/{obiewId}")
    public ResponseEntity<Boolean> deleteById(@PathVariable String obiewId) {
        Obiew obiew = obiewRepository.findByObiewId(obiewId);
        if (obiew != null) {
            obiewRepository.deleteById(obiewId);
            return new ResponseEntity<>(true, HttpStatus.OK);
        } else {
            return new ResponseEntity<>(false, HttpStatus.FORBIDDEN);
        }
    }
}
